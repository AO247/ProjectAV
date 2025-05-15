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
    camera->SetLocalPosition({ GetOwner()->GetLocalPosition().x, GetOwner()->GetLocalPosition().y + height * 9 / 10, GetOwner()->GetLocalPosition().z });
    GetOwner()->SetLocalRotation({ 0.0f, camera->GetLocalRotationEuler().y, 0.0f });
    KeyboardInput();
    MovePlayer();
    /*Positioning();
    if (!wnd.CursorEnabled())
    {
        GroundCheck();
        Cooldowns(dt);
		KeyboardInput();
		SpeedControl();
		MovePlayer();
    }*/
}


void PlayerController::SpeedControl()
{
	/*if (dashed) return;
    Vector3 velocity(rigidbody->GetVelocity().x, 0.0f, rigidbody->GetVelocity().z);

    if (velocity.Length() > moveSpeed) 
    {
		velocity.Normalize();
        Vector3 limitedVel = velocity * moveSpeed;
		rigidbody->SetVelocity(Vector3(limitedVel.x, rigidbody->GetVelocity().y, limitedVel.z));
    }*/
}

void PlayerController::Jump()
{
    //rigidbody->AddForce(Vector3(0.0f, jumpForce * 1000.0f, 0.0f));
    rigidbody->GetBulletRigidbody()->applyForce(btVector3(0, jumpForce * 200.0f, 0), btVector3(0, 0, 0));
    /*if ((grounded || !doubleJumped) && !jumped) {
        rigidbody->SetVelocity(Vector3(rigidbody->GetVelocity().x, 0.0f, rigidbody->GetVelocity().z));
        rigidbody->AddForce(Vector3(0.0f, jumpForce * 1000.0f, 0.0f));
        if (grounded) {
			grounded = false;
        }
        else {
			doubleJumped = true;
        }
		jumped = true;
    }*/
}

void PlayerController::Dash()
{
    if (!canDash) return;
    dashed = true;
	canDash = false;
    
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
    //dashDirection = camera->Back();
    dashDirection.Normalize();

    dashTimer = 0.3f;
	dashCooldownTimer = dashCooldown;

	/*rigidbody->friction = false;
    rigidbody->SetVelocity({ 0.0f, 0.0f, 0.0f });
    rigidbody->AddForce(dashDirection * dashForce * 100.0f);*/
}

void PlayerController::GroundCheck()
{
    //grounded = rigidbody->grounded;
    if (grounded)
    {
        doubleJumped = false;
    }
}

void PlayerController::MovePlayer()
{
    moveDirection.Normalize();

    Vector3 movementVector = moveDirection * moveSpeed * 1000.0f;

    //btVector3 newVelocity = rigidbody->GetBulletRigidbody()->getLinearVelocity() + btVector3(movementVector.x, movementVector.y, movementVector.z);
    //rigidbody->GetBulletRigidbody()->setLinearVelocity(newVelocity);
    rigidbody->GetBulletRigidbody()->applyForce(btVector3(movementVector.x, movementVector.y, movementVector.z), btVector3(0, 0, 0));
    //rigidbody->AddForce(moveDirection * moveSpeed * 1000.0f);
}

void PlayerController::Ability1()
{
    /*CapsuleCollider* col = ability1->GetComponent<CapsuleCollider>();
    std::vector<Collider*> colliders = col->GetTriggerStay();
	for (Collider* collider : colliders)
	{
		if (collider->GetOwner()->tag == "Enemy" || collider->GetOwner()->tag == "Stone")
		{
			collider->GetOwner()->GetComponent<Rigidbody>()->SetVelocity(Vector3(0.0f, 0.0f, 0.0f));
			collider->GetOwner()->GetComponent<Rigidbody>()->AddForce(ability1->Forward() * 100000.0f);
		}
	}*/
}

void PlayerController::Ability2()
{
    
	/*BoundingSphere* col = ability2->GetComponent<BoundingSphere>();

    std::vector<Collider*> colliders = col->GetTriggerStay();

    for (Collider* collider : colliders)
    {
        if (collider->GetOwner()->tag == "Enemy" || collider->GetOwner()->tag == "Stone")
        {
            collider->GetOwner()->GetComponent<Rigidbody>()->SetVelocity(Vector3(collider->GetOwner()->GetComponent<Rigidbody>()->GetVelocity().x,
                0.0f, collider->GetOwner()->GetComponent<Rigidbody>()->GetVelocity().z));
            collider->GetOwner()->GetComponent<Rigidbody>()->AddForce(Vector3(0.0f, 70000.0f, 0.0f));
        }
    }*/


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
        if (!pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying()) {
            pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
        }
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
    ImGui::Checkbox("Jumped", &jumped);
	ImGui::Checkbox("CanDash", &canDash);
	ImGui::Checkbox("Grounded", &grounded);

	ImGui::InputFloat("Dash Cooldown", &dashCooldown);
}