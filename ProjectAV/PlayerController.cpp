#include "PlayerController.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm> 
#include <string>
#include "Components.h"

namespace dx = DirectX;
PlayerController::PlayerController(Node* owner, Window& window)
    : Component(owner), wnd(window)  
{
	rigidbody = owner->GetComponent<Rigidbody>();
	camera = owner->GetRoot()->FindFirstChildByTag("CAMERA");
}


void PlayerController::Update(float dt)
{
    Positioning();
    Cooldowns(dt);
    if (!wnd.CursorEnabled() && alive && !wnd.playerLocked)
    {
        AutoJump();
        PlayerGroundCheck();
		KeyboardInput();
		SpeedControl();
		MovePlayer(dt);
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
        if (grounded) {
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3(0.0f, jumpForce, 0.0f));
			grounded = false;
            if (pOwner->GetComponent<SoundEffectsPlayer>()) {

                pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
            }
        }
        else {
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3(0.0f, secondJumpForce, 0.0f));
			doubleJumped = true;
            if (pOwner->GetComponent<SoundEffectsPlayer>()) {

                pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
            }
        }
		jumped = true;
    }
}

void PlayerController::Dash()
{
    if (!canDash) return;
    dashed = true;
	canDash = false;
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
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

void PlayerController::PlayerGroundCheck()
{
    grounded = false;

	Vector3 playerPos = GetOwner()->GetWorldPosition();
    Vector3 pos = playerPos;
    Vec3 direction = Vec3(0.0f, -(height / 2 + 0.2f), 0.0f);
    RRayCast rayCenter = RRayCast(
        RVec3(pos.x, pos.y, pos.z),
        direction
    );
    RayCastResult resultCenter;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayCenter, resultCenter,
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }), 
        IgnoreMultipleObjectLayerFilter({Layers::PLAYER, Layers::TRIGGER})))
    {
        grounded = true;
    }

    pos = playerPos + -moveDirection;

    RRayCast rayBack = RRayCast(
        RVec3(pos.x, pos.y, pos.z),
        direction
    );
    RayCastResult resultBack;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayBack, resultBack,
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
        IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER })))
    {
        grounded = true;
    }

    pos = playerPos + moveDirection;

    RRayCast rayForward = RRayCast(
        RVec3(pos.x, pos.y, pos.z),
        direction
    );
    RayCastResult resultForward;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayForward, resultForward,
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
        IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER })))
    {
        grounded = true;
    }


    if (grounded)
    {
        doubleJumped = false;
    }
}

void PlayerController::MovePlayer(float dt)
{
    if (grounded)
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 1000.0f * dt);
    }
    else
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 100.0f * dt);
    }
}

void PlayerController::AutoJump()
{
    if (grounded)
    {
        Vector3 playerPos = GetOwner()->GetWorldPosition();
        Vector3 pos = playerPos + moveDirection * autoJumpRange;
        RRayCast rayFront = RRayCast(
            RVec3(pos.x, pos.y, pos.z),
            Vec3(0.0f, -(height / 2 + 0.2f), 0.0f)
        );
        RayCastResult resultFront;
        if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayFront, resultFront,
            IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER })))
        {
            Vec3 hitPos = rayFront.mOrigin + rayFront.mDirection * resultFront.mFraction;
            Vec3 playerDownPos = Vec3(playerPos.x, playerPos.y - (height / 2), playerPos.z);
            float diffrence = (hitPos.GetY() - playerDownPos.GetY());
            if (diffrence < 3.0f && diffrence > 0.5f)
            {
				PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(playerPos.x, playerPos.y + diffrence, playerPos.z), EActivation::Activate);
            }
        }
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
}

void PlayerController::Positioning()
{
    camera->SetLocalPosition({ GetOwner()->GetLocalPosition().x, GetOwner()->GetLocalPosition().y + height * 9 / 10, GetOwner()->GetLocalPosition().z });

    Quat q = Quat::sEulerAngles(Vec3(0.0f, camera->GetLocalRotationEuler().y, 0.0f));
    PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(rigidbody->GetBodyID(), q, EActivation::Activate);
}


void PlayerController::KeyboardInput()
{

    while (const auto e = wnd.mouse.PollEvent())
    {
        switch (e->GetType())
        {
        case Mouse::Event::Type::LPress:
            abilitySlot1->GetComponent<Ability>()->Pressed();
            if (pOwner->GetComponent<SoundEffectsPlayer>()) {
                float p = (rand() % 2) + 1;
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(p);
            }
            break;

        case Mouse::Event::Type::RPress:
            abilitySlot2->GetComponent<Ability>()->Pressed();
            if (pOwner->GetComponent<SoundEffectsPlayer>()) {
                float p = (rand() % 2) + 3;
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(p);
            }
            break;

        case Mouse::Event::Type::LRelease:
            abilitySlot1->GetComponent<Ability>()->Released();
            break;

        case Mouse::Event::Type::RRelease:
            abilitySlot2->GetComponent<Ability>()->Released();
            break;
        }

    }

    if (wnd.kbd.IsKeyPressed('Q'))
    {
        abilitySlot3->GetComponent<Ability>()->Pressed();
	}


    moveDirection = Vector3(0.0f, 0.0f, 0.0f);
    if (wnd.kbd.IsKeyPressed(VK_SPACE))
    {
        Jump();
    }
    else {
        jumped = false;
    }
    if (dashed) return;

    if (wnd.kbd.IsKeyPressed('W'))
    {
        moveDirection += GetOwner()->Forward();
    }
    if (wnd.kbd.IsKeyPressed('S'))
    {
        moveDirection += GetOwner()->Back();
    }
    if (wnd.kbd.IsKeyPressed('A'))
    {
        moveDirection += GetOwner()->Left();
    }
    if (wnd.kbd.IsKeyPressed('D'))
    {
        moveDirection += GetOwner()->Right();
    }
    moveDirection.Normalize();

    if (wnd.kbd.IsKeyPressed(VK_SHIFT))
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
    ImGui::InputFloat("Height", &height);
    ImGui::Checkbox("Jumped", &jumped);
    ImGui::Checkbox("CanDash", &canDash);
    ImGui::Checkbox("Grounded", &grounded);
    ImGui::Checkbox("Double Jumped", &doubleJumped);
	ImGui::Checkbox("Alive", &alive);
    ImGui::InputFloat("Auto Jump Range", &autoJumpRange);


}