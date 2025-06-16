////////////////////////////////
// Evolve of ability 1        //
// Pick item then pick a spot //
////////////////////////////////
#include "Ability6.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm> 
#include <string>

namespace dx = DirectX;
Ability6::Ability6(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{
    player = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
}


void Ability6::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        Positioning();
        Cooldowns(dt);
        if (isPressed)
        {
            Pulling(dt);
        }
    }
}
void Ability6::Positioning()
{
    if (isPressed) return;
    Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
    Vec3 direction = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
    RRayCast ray = RRayCast(position, direction * 100.0f);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
        MultipleBroadPhaseLayerFilter({BroadPhaseLayers::WALL }),
        MultipleObjectLayerFilter({Layers::WALL })))
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
void Ability6::Pulling(float dt)
{
    Vector3 cameraPos = camera->GetWorldPosition();
    Vector3 targetPosition = cameraPos + camera->Forward() * 8.0f;
    Rigidbody* rb = selectedNode->GetComponent<Rigidbody>();
    
    Vector3 tempDir = targetPosition - selectedNode->GetWorldPosition();
    tempDir.y = 0.0f;
    if (tempDir.Length() < 3.0f)
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rb->GetBodyID(), Vec3::sZero());
        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rb->GetBodyID(), Vec3(targetPosition.x, targetPosition.y, targetPosition.z), EActivation::Activate);
    }
    else
    {
        Vector3 dir = targetPosition - selectedNode->GetWorldPosition();
        dir.y *= 10.0f;
        dir.Normalize();
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rb->GetBodyID(), Vec3(dir.x, dir.y, dir.z) * force * 100.0f * dt);
    }
}
void Ability6::Pressed()
{
    if (!abilityReady) return;
    if (selectedNode == nullptr) return;
    isPressed = true;
    leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
    leftHandNormal->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
}
void Ability6::Released()
{
    if (!isPressed) return;
    isPressed = false;
    if (!abilityReady) return;
    leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 4.0f });
    timeToChange = 0.3f;
   
    baseAbility->Pressed();

    cooldownTimer = cooldown;
    abilityReady = false;
}


void Ability6::Cooldowns(float dt)
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


void Ability6::DrawImGuiControls()
{
    ImGui::InputFloat("Force", &force);

    ImGui::InputFloat("Cooldown", &cooldown);

}