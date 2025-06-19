////////////////////////////////
// Evolve of ability 1        //
// Pick item then pick a spot //
////////////////////////////////
#include "Ability4.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm> 
#include <string>

namespace dx = DirectX;
Ability4::Ability4(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{
    player = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
}


void Ability4::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        if (isPressed)
        {
			pressedTime += dt;
        }
        Positioning();
        Cooldowns(dt);
    }
}
void Ability4::Positioning()
{
    if (isPressed) return;
    Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
    Vec3 direction = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
    RRayCast ray = RRayCast(position, direction * 100.0f);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
        MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::WALL }),
        MultipleObjectLayerFilter({ Layers::WALL })))
    {
        position = ray.mOrigin + ray.mDirection * result.mFraction;
        if (PhysicsCommon::physicsSystem->GetBodyInterface().GetMotionType(result.mBodyID) == EMotionType::Dynamic)
        {
            selectedNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(result.mBodyID));
        }
    }
    else
    {
        selectedNode = nullptr;
    }
}
void Ability4::Pressed()
{
    if (!abilityReady) return;
    isPressed = true;
    pressedTime = 0.0f;
    leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
    leftHandNormal->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
    cameraRotation = camera->GetLocalRotationEuler();
}
void Ability4::Released()
{
    if (!isPressed) return;
    isPressed = false;
    if (!abilityReady) return;
    leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 4.0f });
    timeToChange = 0.3f;

    if (pressedTime < 0.2f || selectedNode == nullptr)
    {
        pressedTime = 0.0f;
        baseAbility->Pressed();
    }
    else
    {
        pressedTime = 0.0f;
        Vector3 direction = Vector3::Zero;
        Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
        Vec3 dire = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
        RRayCast ray = RRayCast(position, dire * 100.0f);
        RayCastResult result;
        if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
            IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER })))
        {
            position = ray.mOrigin + ray.mDirection * result.mFraction;
            Vector3 pos = Vector3(position.GetX(), position.GetY(), position.GetZ());
            direction = pos - selectedNode->GetWorldPosition();
            direction.Normalize();
        }
        else
        {
            Vector3 dir = camera->GetWorldPosition() - selectedNode->GetWorldPosition();

            float yaw = std::atan2(dir.x, dir.z);
            float distanceXZ = std::sqrt(dir.x * dir.x + dir.z * dir.z);
            float pitch = -std::atan2(dir.y, distanceXZ); // Negative for DirectX

            pOwner->SetLocalRotation(DirectX::XMFLOAT3(pitch, yaw, 0.0f));

            Vector3 newCameraRot = camera->GetLocalRotationEuler();

            float deltaYaw = wrap_angle(newCameraRot.y - cameraRotation.y);
            float deltaPitch = wrap_angle(newCameraRot.x - cameraRotation.x);


            direction = pOwner->Right() * -deltaYaw + pOwner->Up() * -deltaPitch;

            if (direction.Length() < 0.01f)
                direction = pOwner->Back();

            direction.Normalize();
        }

        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(selectedNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
        PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(selectedNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(direction.x, direction.y, direction.z) * force);

    }
    cooldownTimer = cooldown;
    abilityReady = false;
}


void Ability4::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
    }
    else
    {
        if (!abilityReady)
        {
            leftHandNormal->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
        }
        abilityReady = true;
    }
    if (timeToChange > 0.0f)
    {
        timeToChange -= dt;
        if (timeToChange <= 0.0f)
        {
            leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
            leftHandNormal->SetLocalPosition({ 0.0f, -2.7f, 1.0f });
        }
    }

}

void Ability4::DrawImGuiControls()
{
    ImGui::InputFloat("Force", &force);
    ImGui::InputFloat("Cooldown", &cooldown);

}