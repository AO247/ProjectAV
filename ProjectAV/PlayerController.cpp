#include "PlayerController.h"
#include "Node.h"       // Include Node to call SetLocalPosition/Rotation
#include "Window.h"     // Included via header, but good practice
#include "CMath.h"      // For wrap_angle and PI (ensure this is included)
#include <DirectXMath.h>
#include <algorithm> // for std::clamp
#include "Raycast.h"
#include <string>
#include "CapsuleCollider.h"
#include "BoundingSphere.h"
#include "Components.h"

namespace dx = DirectX;
PlayerController::PlayerController(Node* owner, Window& window)
    : Component(owner), wnd(window)  // Initialize reference member
{
	rigidbody = owner->GetComponent<Rigidbody>();
	camera = owner->GetRoot()->FindFirstChildByTag("CAMERA");
}


void PlayerController::Update(float dt)
{
    Positioning();
    if (!wnd.CursorEnabled() && alive)
    {
        GroundCheck();
        Cooldowns(dt);
		KeyboardInput();
		SpeedControl();
		MovePlayer();
    }
}


void PlayerController::SpeedControl()
{
	if (dashed) return;
	Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	velocity.SetY(0.0f);
	if (grounded) {
        if (velocity.Length() > moveSpeed)
        {
            velocity = velocity.Normalized();
            velocity *= moveSpeed;
            Vec3 limitedVel = velocity;
            limitedVel.SetY(PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID()).GetY());
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), limitedVel);
        }
    }
	else {
        if (velocity.Length() > (moveSpeed * airMultiplier))
        {
            velocity = velocity.Normalized();
            velocity *= moveSpeed * airMultiplier;
            Vec3 limitedVel = velocity;
            limitedVel.SetY(PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID()).GetY());
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), limitedVel);
        }
	}

	if (moveDirection == Vector3(0.0f, 0.0f, 0.0f))
	{
		Vec3 vel = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
        if (grounded) {
            vel.SetX(vel.GetX() * 0.90f);
            vel.SetZ(vel.GetZ() * 0.90f);
        }
        else {
            vel.SetX(vel.GetX() * 0.95f);
            vel.SetZ(vel.GetZ() * 0.95f);
        }
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), vel);
	}
}

void PlayerController::Jump()
{
    if ((grounded || !doubleJumped) && !jumped) {
		Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
        if (velocity.GetY() < 0.0f)
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3(velocity.GetX(), 0.0f, velocity.GetZ()));

        }
		PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3(0.0f, jumpForce, 0.0f));
        if (grounded) {
			grounded = false;
        }
        else {
			doubleJumped = true;
        }
		jumped = true;
    }
}

void PlayerController::Dash()
{
    if (!canDash) return;
    dashed = true;
	canDash = false;
    if (!pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying()) {
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
    }
    Vector3 dashDirection = moveDirection;
    if (moveDirection == pOwner->Forward())
    {
		dashDirection = camera->Forward();
    }
    else if (moveDirection == pOwner->Back())
    {
        dashDirection = camera->Back();
    }

    if (moveDirection == Vector3(0.0f, 0.0f, 0.0f))
    {
		dashDirection = camera->Forward();
    }

    dashDirection.Normalize();
    Vec3 dir = Vec3(dashDirection.x, dashDirection.y, dashDirection.z);
    dashTimer = 0.2f;
	dashCooldownTimer = dashCooldown;

	PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.0f);
    PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), dir * dashForce);

}

void PlayerController::GroundCheck()
{
    RRayCast ray = RRayCast(
        RVec3(GetOwner()->GetWorldPosition().x, GetOwner()->GetWorldPosition().y, GetOwner()->GetWorldPosition().z),
        Vec3(0.0f, -(height / 2 + 0.2f), 0.0f)
    );
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, 
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }), 
        IgnoreMultipleObjectLayerFilter({Layers::PLAYER, Layers::TRIGGER})))
    {
		grounded = true;
    }
    else
    {
		grounded = false;
    }

    if (grounded)
    {
        doubleJumped = false;
    }
}

void PlayerController::MovePlayer()
{
    moveDirection.Normalize();
    if (grounded)
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 100.0f);
    }
    else
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 10.0f);
    }
}

void PlayerController::Cooldowns(float dt)
{
    if (dashTimer > 0.0f)
    {
        dashTimer -= dt;
    }
    else
    {
        dashed = false;
        //rigidbody->friction = true;
        PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.1f);
    }
    if (dashCooldownTimer > 0.0f)
    {
        dashCooldownTimer -= dt;
    }
    else
    {
        canDash = true;
    }
	if (ability1CooldownTimer > 0.0f)
	{
		ability1CooldownTimer -= dt;
	}
	else
	{
		ability1Ready = true;
	}
	if (ability2CooldownTimer > 0.0f)
	{
		ability2CooldownTimer -= dt;
	}
	else
	{
		ability2Ready = true;
	}
}

void PlayerController::Positioning()
{
    camera->SetLocalPosition({ GetOwner()->GetLocalPosition().x, GetOwner()->GetLocalPosition().y + height * 9 / 10, GetOwner()->GetLocalPosition().z });
    //GetOwner()->SetLocalRotation({ 0.0f, camera->GetLocalRotationEuler().y, 0.0f });
    Quat q = Quat::sEulerAngles(Vec3(0.0f, camera->GetLocalRotationEuler().y, 0.0f));
    PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(rigidbody->GetBodyID(), q, EActivation::Activate);
}


void PlayerController::KeyboardInput()
{
    while (const auto e = wnd.mouse.Read()) // Read events from the queue
    {
        switch (e->GetType())
        {
        case Mouse::Event::Type::LPress:
            ability1->GetComponent<Ability1>()->Active();
            break;

        case Mouse::Event::Type::RPress:
            ability2->GetComponent<Ability2>()->Active();
            break;
        }
    }

    moveDirection = Vector3(0.0f, 0.0f, 0.0f);
    if (wnd.kbd.KeyIsPressed(VK_SPACE))
    {
        Jump();
    }
    else {
        jumped = false;
    }
    if (dashed) return;

    if (wnd.kbd.KeyIsPressed('W'))
    {
        moveDirection += GetOwner()->Forward();
    }
    if (wnd.kbd.KeyIsPressed('S'))
    {
        moveDirection += GetOwner()->Back();
    }
    if (wnd.kbd.KeyIsPressed('A'))
    {
        moveDirection += GetOwner()->Left();
    }
    if (wnd.kbd.KeyIsPressed('D'))
    {
        moveDirection += GetOwner()->Right();
    }


    if (wnd.kbd.KeyIsPressed(VK_SHIFT))
    {
        Dash();
    }
}
void PlayerController::DrawImGuiControls()
{
    ImGui::InputFloat("Move Speed", &moveSpeed);
    ImGui::InputFloat("JumpForce", &jumpForce);
	ImGui::InputFloat("Dash Force", &dashForce);
	ImGui::InputFloat("Dash Cooldown", &dashCooldown);
	ImGui::InputFloat("Ability1 Cooldown", &ability1Cooldown);
	ImGui::InputFloat("Ability2 Cooldown", &ability2Cooldown);
    ImGui::InputFloat("Height", &height);
    ImGui::Checkbox("Jumped", &jumped);
    ImGui::Checkbox("CanDash", &canDash);
    ImGui::Checkbox("Grounded", &grounded);
    ImGui::Checkbox("Double Jumped", &doubleJumped);
	ImGui::Checkbox("Alive", &alive);
}