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

namespace dx = DirectX;
PlayerController::PlayerController(Node* owner, Window& window)
    : Component(owner), wnd(window)  // Initialize reference member
{
	rigidbody = owner->GetComponent<Rigidbody>();
    //rigidbody->frictionDamping = 12.0f;
	camera = owner->GetRoot()->FindFirstChildByTag("Camera");
	ability1 = owner->GetRoot()->FindFirstChildByTag("Ability1");
	ability2 = owner->GetRoot()->FindFirstChildByTag("Ability2");
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
        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3(velocity.GetX(), 0.0f, velocity.GetZ()));

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


	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.0f);
    PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), dir * dashForce);

}

void PlayerController::GroundCheck()
{
    RRayCast ray = RRayCast(
        RVec3(GetOwner()->GetWorldPosition().x, GetOwner()->GetWorldPosition().y, GetOwner()->GetWorldPosition().z),
        Vec3(0.0f, -(height/2 + 0.0f), 0.0f)
	);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND),SpecifiedObjectLayerFilter(Layers::GROUND)))
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
//    PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 200.0f);
    if (grounded)
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 1000.0f);
    }
    else
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * moveSpeed * 50.0f);
    }
    //auto& bodyIf = PhysicsCommon::physicsSystem->GetBodyInterface();
    //Vec3 currVel = bodyIf.GetLinearVelocity(rigidbody->GetBodyID());

    //// 1) Zachowaj Y i zeruj poziome
    //float velY = currVel.GetY();
    //currVel.SetY(0.0f);

    //// 2) Desired velocity
    //Vector3 desiredDir = moveDirection;
    //if (desiredDir.LengthSquared() > 1e-6f) desiredDir.Normalize();
    //float targetSpeed = moveSpeed * (grounded ? 1.0f : airMultiplier);
    //Vec3 desiredVel{ desiredDir.x * targetSpeed,
    //                 0.0f,
    //                 desiredDir.z * targetSpeed };

    //// 3) Sprawdü dot(currVel, desiredVel)
    //float dot = currVel.Dot(desiredVel);

    //if (grounded)
    //{
    //    // ziemia: natychmiast
    //    currVel = desiredVel;
    //}
    //else
    //{
    //    if (dot < 0.0f)
    //    {
    //        // hamujemy poziomπ prÍdkoúÊ do zera szybciej, gdy chcemy odwrÛciÊ
    //        constexpr float airBrake = 0.99f; // im bliøej 0, tym wolniej
    //        currVel *= airBrake;
    //    }
    //    else
    //    {
    //        // normalna powietrzna kontrola
    //        constexpr float airControl = 0.99f;
    //        currVel = currVel + (desiredVel - currVel) * airControl;
    //    }
    //}

    //// 4) PrzywrÛÊ Y i ustaw
    //currVel.SetY(velY);
    //bodyIf.SetLinearVelocity(rigidbody->GetBodyID(), currVel);
}

void PlayerController::Ability1()
{
	/*if (!ability1Ready) return;
    CapsuleCollider* col = ability1->GetComponent<CapsuleCollider>();
    std::vector<Collider*> colliders = col->GetTriggerStay();
	for (Collider* collider : colliders)
	{
		if (collider->GetOwner()->tag == "Enemy" || collider->GetOwner()->tag == "Stone")
		{
			collider->GetOwner()->GetComponent<Rigidbody>()->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			collider->GetOwner()->GetComponent<Rigidbody>()->AddForce(ability1->Forward() * 100000.0f);
		}
	}
	ability1CooldownTimer = ability1Cooldown;
	ability1Ready = false;*/
}

void PlayerController::Ability2()
{
	/*if (!ability2Ready) return;
	BoundingSphere* col = ability2->GetComponent<BoundingSphere>();
    std::vector<Collider*> colliders = col->GetTriggerStay();

    for (Collider* collider : colliders)
    {
        if (collider->GetOwner()->tag == "Enemy" || collider->GetOwner()->tag == "Stone")
        {
            collider->GetOwner()->GetComponent<Rigidbody>()->SetVelocity(Vector3(collider->GetOwner()->GetComponent<Rigidbody>()->GetVelocity().x,
                0.0f, collider->GetOwner()->GetComponent<Rigidbody>()->GetVelocity().z));
            collider->GetOwner()->GetComponent<Rigidbody>()->AddForce(Vector3(0.0f, 70000.0f, 0.0f));
        }
    }
	ability2CooldownTimer = ability2Cooldown;
    ability2Ready = false;*/
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
    GetOwner()->SetLocalRotation({ 0.0f, camera->GetLocalRotationEuler().y, 0.0f });
    ability1->SetLocalPosition(camera->GetLocalPosition());
    ability1->SetLocalRotation(camera->GetLocalRotationEuler());

    /*RaycastData rayData = Raycast::CastThroughLayers(camera->GetWorldPosition(), camera->Forward(), std::vector<Layers>{PLAYER});

    if (rayData.hitCollider != nullptr)
    {
        if (rayData.hitCollider->GetOwner()->tag == "Ground")
        {
            ability2->SetLocalPosition(rayData.hitPoint);
        }
        else
        {
            RaycastData rayData2 = Raycast::CastAtLayers(rayData.hitCollider->GetOwner()->GetWorldPosition(), Vector3(0.0f, -1.0f, 0.0f), std::vector<Layers>{GROUND});
			if (rayData2.hitCollider)
			{
                if (rayData2.hitCollider->GetOwner()->tag == "Ground")
                {
                    ability2->SetLocalPosition(rayData2.hitPoint);
                }
			}
        }
    }*/


}


void PlayerController::KeyboardInput()
{
    moveDirection = Vector3(0.0f, 0.0f, 0.0f);

    while (const auto e = wnd.mouse.Read()) // Read events from the queue
    {
        switch (e->GetType())
        {
        case Mouse::Event::Type::LPress:
            Ability1();
            break;

        case Mouse::Event::Type::RPress:
            Ability2();
            break;
        }
    }
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