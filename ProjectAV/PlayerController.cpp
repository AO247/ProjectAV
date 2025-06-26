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
        MovePlayer(dt);
		SpeedControl(dt);
    }
}


void PlayerController::SpeedControl(float dt)
{
	if (dashed) return;
	Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	velocity.SetY(0.0f);
	if (grounded) {
        if (velocity.Length() > maxSpeed)
        {
            velocity = velocity.Normalized();
            velocity *= maxSpeed;
            Vec3 limitedVel = velocity;
            limitedVel.SetY(PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID()).GetY());
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), limitedVel);
        }
    }
	else {
        if (velocity.Length() > (maxSpeed * airMultiplier))
        {
            velocity = velocity.Normalized();
            velocity *= maxSpeed * airMultiplier;
            Vec3 limitedVel = velocity;
            limitedVel.SetY(PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID()).GetY());
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), limitedVel);
        }
	}

	if (moveDirection == Vector3(0.0f, 0.0f, 0.0f))
	{
		Vec3 vel = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
        if (grounded) {
            float dampingFactor = powf(groundDamping, dt);
            vel.SetX(vel.GetX() * dampingFactor * dampingFactor); // 0.92
            vel.SetZ(vel.GetZ() * dampingFactor * dampingFactor);
        }
        else {
            float dampingFactor = powf(airDamping, dt);
            vel.SetX(vel.GetX() * dampingFactor);
            vel.SetZ(vel.GetZ() * dampingFactor);
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
				int randSound = rand() % 2;
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
            }
        }
        else {
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), Vec3(0.0f, secondJumpForce, 0.0f));
			doubleJumped = true;

            //doublejump
            leftHand->GetComponent<AnimationComponent>()->PlayAnimation(10, 0.3f, false);
            rightHand->GetComponent<AnimationComponent>()->PlayAnimation(10, 0.3f, false);


            if (pOwner->GetComponent<SoundEffectsPlayer>()) {
				int randSound = rand() % 2 + 2;
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
            }
        }
		jumped = true;
    }
}

void PlayerController::Dash()
{
    if (!canDash) return;

    //dash
    leftHand->GetComponent<AnimationComponent>()->PlayAnimation(9, 0.3f, false);
    rightHand->GetComponent<AnimationComponent>()->PlayAnimation(9, 0.3f, false);


    dashed = true;
	canDash = false;
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
		int randSound = rand() % 2 + 4;
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(5, 1.0f, false);
    }
    Vector3 dashDirection = moveDirection;
    if (evolvedDash)
    {
        if (moveDirection == pOwner->Forward())
        {
            dashDirection = camera->Forward();
        }
        else if (moveDirection == pOwner->Back())
        {
            dashDirection = camera->Back();
        }
    }
    if (moveDirection == Vector3(0.0f, 0.0f, 0.0f))
    {
        if (evolvedDash)
        {
            dashDirection = camera->Forward();
        }
        else
        {
            dashDirection = pOwner->Forward();
        }
    }
    dashDirection.y += 0.3f;
    dashDirection.Normalize();
    Vec3 dir = Vec3(dashDirection.x, dashDirection.y, dashDirection.z);
    dashTimer = 0.2f;
	dashCooldownTimer = dashCooldown;
	Vec3 vel = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
    if(vel.GetY() < 0.0f)
    {
        vel.SetY(vel.GetY() / 2.0f);
    }
    else
    {
        vel.SetY(0.0f);
	}
	PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), Vec3(0.0f, vel.GetY(), 0.0f));
	PhysicsCommon::physicsSystem->GetBodyInterface().SetFriction(rigidbody->GetBodyID(), 0.0f);
    PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rigidbody->GetBodyID(), dir * dashForce);

}

void PlayerController::PlayerGroundCheck()
{
    if (leftHand->GetComponent<AnimationComponent>()->GetCurrentPlayingAnimationRaw() == nullptr) {
        leftHand->GetComponent<AnimationComponent>()->PlayAnimation(13);
    }
    if (rightHand->GetComponent<AnimationComponent>()->GetCurrentPlayingAnimationRaw() == nullptr) {
        rightHand->GetComponent<AnimationComponent>()->PlayAnimation(13);
    }
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
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
        IgnoreMultipleObjectLayerFilter({ Layers::ENEMY, Layers::PLAYER, Layers::TRIGGER})))
    {
        grounded = true;
    }

    pos = playerPos + -moveDirection * 1.6f;

    RRayCast rayBack = RRayCast(
        RVec3(pos.x, pos.y, pos.z),
        direction
    );
    RayCastResult resultBack;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayBack, resultBack,
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::ENEMY, BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
        IgnoreMultipleObjectLayerFilter({ Layers::ENEMY,  Layers::PLAYER, Layers::TRIGGER })))
    {
        grounded = true;
    }

    pos = playerPos + moveDirection * 1.6;

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

    if (grounded && moveDirection.Length() > 0.0f)
    { 
        Vec3 desiredVel = Vec3(moveDirection.x, 0.0f, moveDirection.z);
        Vec3 currentVel = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
        Vec3 copy = currentVel;
        currentVel.SetY(0);
        float currentLength = currentVel.Length();
        currentVel = currentVel.Normalized();

        if ((desiredVel - currentVel).Length() > 1.9f)
        {
            //Vec3 vel = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
            float dampingFactor = powf(groundDamping, dt);
            copy.SetX(copy.GetX() * dampingFactor); // 0.92
            copy.SetZ(copy.GetZ() * dampingFactor);
            //copy.SetX(copy.GetX() * 0.92f);
            //copy.SetZ(copy.GetZ() * 0.92f);
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), copy);

        }
       /* else if ((desiredVel - currentVel).Length() > 1.0f)
        {
            copy.SetX(copy.GetX() * 0.98f);
            copy.SetZ(copy.GetZ() * 0.98f);
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), copy);

        }
        else if ((desiredVel - currentVel).Length() > 1.2f)
        {
            copy.SetX(copy.GetX() * 0.96f);
            copy.SetZ(copy.GetZ() * 0.96f);
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), copy);

        }*/
        else if ((desiredVel - currentVel).Length() > 0.2f)
        {
            Vec3 newVelocity = desiredVel * currentLength;
            newVelocity.SetY(copy.GetY());
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rigidbody->GetBodyID(), newVelocity);
        }
        if (slowTime > 0.0f)
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), 
                Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * acceleration * slowPower * 1000.0f * dt);
        }
        else
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rigidbody->GetBodyID(), 
                Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * acceleration * 1000.0f * dt);

        }
    }
    else
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(
            rigidbody->GetBodyID(),
            Vec3Arg(moveDirection.x, moveDirection.y, moveDirection.z) * acceleration * 800.0f * dt
        );
    }
    stepSoundTimer -= dt;
    if (stepSoundTimer <= 0.0f && moveDirection.Length() > 0.0f && grounded)
    {
        if (pOwner->GetComponent<SoundEffectsPlayer>())
        {
            int randSound = rand() % 6 + 16;
            pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 0.6f);
        }
        stepSoundTimer = stepSoundInterval;
    }
}

void PlayerController::AutoJump()
{
    if (grounded)
    {
        Vector3 playerPos = GetOwner()->GetWorldPosition();

        float angleRad = DirectX::XMConvertToRadians(30.0f);

        Vector3 rightDir = Vector3(
            moveDirection.x * cosf(angleRad) - moveDirection.z * sinf(angleRad),
            moveDirection.y,
            moveDirection.x * sinf(angleRad) + moveDirection.z * cosf(angleRad)
        );
        Vector3 leftDir = Vector3(
            moveDirection.x * cosf(-angleRad) - moveDirection.z * sinf(-angleRad),
            moveDirection.y,
            moveDirection.x * sinf(-angleRad) + moveDirection.z * cosf(-angleRad)
        );

        Vector3 pos = playerPos + moveDirection * autoJumpRange;
        Vector3 leftPos = playerPos + leftDir * autoJumpRange;
        Vector3 rightPos = playerPos + rightDir * autoJumpRange;

        Vector3 posInside = playerPos + moveDirection * (autoJumpRange + 0.5f);
        Vector3 leftPosInside = playerPos + leftDir * (autoJumpRange + 0.5f);
        Vector3 rightPosInside = playerPos + rightDir * (autoJumpRange + 0.5f);

        pos.y += 10.0f;
        leftPos.y += 10.0f;
        rightPos.y += 1.0f;

        Vec3 direction = Vec3(0.0f, -(height / 2 + 0.2f + 10.0f), 0.0f);

        // Central raycast (original)
        RRayCast rayFront = RRayCast(
            RVec3(pos.x, pos.y, pos.z),
            direction
        );
        RayCastResult resultFront;

        RRayCast rayLeft = RRayCast(
            RVec3(leftPos.x, leftPos.y, leftPos.z),
            direction
        );
        RayCastResult resultLeft;

        RRayCast rayRight = RRayCast(
            RVec3(rightPos.x, rightPos.y, rightPos.z),
            direction
        );
        RayCastResult resultRight;

        if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayFront, resultFront,
            IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER})))
        {
            Vec3 hitPos = rayFront.mOrigin + rayFront.mDirection * resultFront.mFraction;
            Vec3 playerDownPos = Vec3(playerPos.x, playerPos.y - (height / 2), playerPos.z);
            float diffrence = (hitPos.GetY() - playerDownPos.GetY());


            if (diffrence < autoJumpHeight && diffrence > 0.5f)
            {
                RRayCast rayInside = RRayCast(
                    RVec3(posInside.x, posInside.y, posInside.z),
                    direction
                );
                RayCastResult resultInside;
                if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayInside, resultInside,
                    IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
                    IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
                {
                    Vec3 hitPosInside = rayInside.mOrigin + rayInside.mDirection * resultInside.mFraction;
                    float diffrenceInside = (hitPosInside.GetY() - playerDownPos.GetY());
                    if (diffrenceInside < autoJumpHeight && diffrenceInside > 0.5f)
                    {
                        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(playerPos.x, playerPos.y + diffrence, playerPos.z), EActivation::Activate);

                    }

                }

            }
        }
        else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayLeft, resultLeft,
            IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
        {
            Vec3 hitPos = rayLeft.mOrigin + rayLeft.mDirection * resultLeft.mFraction;
            Vec3 playerDownPos = Vec3(playerPos.x, playerPos.y - (height / 2), playerPos.z);
            float diffrence = (hitPos.GetY() - playerDownPos.GetY());
            if (diffrence < autoJumpHeight && diffrence > 0.5f)
            {
                RRayCast rayInside = RRayCast(
                    RVec3(leftPosInside.x, leftPosInside.y, leftPosInside.z),
                    direction
                );
                RayCastResult resultInside;
                if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayInside, resultInside,
                    IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
                    IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
                {
                    Vec3 hitPosInside = rayInside.mOrigin + rayInside.mDirection * resultInside.mFraction;
                    float diffrenceInside = (hitPosInside.GetY() - playerDownPos.GetY());
                    if (diffrenceInside < autoJumpHeight && diffrenceInside > 0.5f)
                    {
                        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(playerPos.x, playerPos.y + diffrence, playerPos.z), EActivation::Activate);

                    }

                }
            }
        }


        else if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayRight, resultRight,
            IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
        {
            Vec3 hitPos = rayRight.mOrigin + rayRight.mDirection * resultRight.mFraction;
            Vec3 playerDownPos = Vec3(playerPos.x, playerPos.y - (height / 2), playerPos.z);
            float diffrence = (hitPos.GetY() - playerDownPos.GetY());
            if (diffrence < autoJumpHeight && diffrence > 0.5f)
            {
                RRayCast rayInside = RRayCast(
                    RVec3(rightPosInside.x, rightPosInside.y, rightPosInside.z),
                    direction
                );
                RayCastResult resultInside;
                if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayInside, resultInside,
                    IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::ENEMY, BroadPhaseLayers::TRIGGER }),
                    IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::ENEMY, Layers::TRIGGER })))
                {
                    Vec3 hitPosInside = rayInside.mOrigin + rayInside.mDirection * resultInside.mFraction;
                    float diffrenceInside = (hitPosInside.GetY() - playerDownPos.GetY());
                    if (diffrenceInside < autoJumpHeight && diffrenceInside > 0.5f)
                    {
                        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rigidbody->GetBodyID(), Vec3(playerPos.x, playerPos.y + diffrence, playerPos.z), EActivation::Activate);

                    }

                }
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
    if (slowTime > 0.0f)
    {
        slowTime -= dt;
    }
}

void PlayerController::Positioning()
{
    camera->SetLocalPosition({ GetOwner()->GetLocalPosition().x, GetOwner()->GetLocalPosition().y + (height/2) * 7.5f / 10, GetOwner()->GetLocalPosition().z });

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
            if (!abilitySlot1->GetComponent<Ability>()->Pressed())
            {
				int randSound = rand() % 2 + 12;
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
            }
            break;

        case Mouse::Event::Type::RPress:
            abilitySlot2->GetComponent<Ability>()->Pressed();
            if (!abilitySlot2->GetComponent<Ability>()->Pressed())
            {
                int randSound = rand() % 2 + 12;
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
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
    while (const auto e = wnd.kbd.PollKeyEvent())
    {
        if (e->IsPress())
        {
            switch (e->GetCode())
            {
            case 'Q':
                abilitySlot3->GetComponent<Ability>()->Pressed();
                break;
            }
        }
        else if (e->IsRelease())
        {
            switch (e->GetCode())
            {
            case 'Q':
                abilitySlot3->GetComponent<Ability>()->Pressed();
                if (!abilitySlot3->GetComponent<Ability>()->Pressed())
                {
                    int randSound = rand() % 2 + 12;
                    pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
                }
                abilitySlot3->GetComponent<Ability>()->Released();
                break;
            }
        }
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
    ImGui::InputFloat("Move Speed", &maxSpeed);
	ImGui::InputFloat("Acceleration", &acceleration);
    ImGui::InputFloat("JumpForce", &jumpForce);
	ImGui::InputFloat("Second Jump Force", &secondJumpForce);
	ImGui::InputFloat("Dash Force", &dashForce);
	ImGui::InputFloat("Dash Cooldown", &dashCooldown);
    ImGui::InputFloat("Height", &height);
    ImGui::Checkbox("Jumped", &jumped);
    ImGui::Checkbox("CanDash", &canDash);
    ImGui::Checkbox("Grounded", &grounded);
    ImGui::Checkbox("Double Jumped", &doubleJumped);
	ImGui::Checkbox("Alive", &alive);
    ImGui::InputFloat("Auto Jump Range", &autoJumpRange);
	ImGui::InputFloat("Auto Jump Height", &autoJumpHeight);
    ImGui::SliderFloat("Air Damping", &airDamping, 0.0f, 0.3f);
    ImGui::SliderFloat("Ground Damping", &groundDamping, 0.0f, 0.3f);

}