////////////////////////////////
// Evolve of ability 2        //
// Throw up for some duration //
////////////////////////////////
#include "Ability5.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm>
#include <string>
#include "PrefabManager.h"
namespace dx = DirectX;
Ability5::Ability5(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{

}


void Ability5::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        Positioning();
        Cooldowns(dt);
    }
}
void Ability5::Positioning()
{
    Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
    Vec3 direction = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
    RRayCast ray = RRayCast(position, direction * 100.0f);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
        IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER })))
    {
        if (PhysicsCommon::physicsSystem->GetBodyInterface().GetObjectLayer(result.mBodyID) != Layers::GROUND)
        {
            Node* body = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(result.mBodyID));
            if (body != nullptr) 
            {
                RRayCast ray2 = RRayCast(Vec3(body->GetWorldPosition().x, body->GetWorldPosition().y, body->GetWorldPosition().z), Vec3(0.0f, -100.0f, 0.0f));
                RayCastResult result2;
                if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray2, result2, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
                {
                    position = ray2.mOrigin + ray2.mDirection * result2.mFraction;
                    pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
                }
            }
        }
        else
        {
            position = ray.mOrigin + ray.mDirection * result.mFraction;
            pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
        }
    }
}
bool Ability5::Pressed()
{
    if (!abilityReady) return false;
    // animacja 
    // dŸwiêk aktywacji
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        float randSound = (rand() % 4);
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
    }

    rightHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
    rightHand->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
    timeToChange = 0.3f;
    PrefabManager::InstantiateAbility5Extend(pOwner->GetParent(), pOwner->GetWorldPosition(), 1.0f, force, duration);
    PrefabManager::InstantiateAbility5Particles(pOwner->GetParent(), pOwner->GetWorldPosition(), 1.0f, { 0,0,0 }, duration);
    cooldownTimer = cooldown;
    cooldownTimer = cooldown;
    abilityReady = false;

    return true;
}
void Ability5::Released()
{
}

void Ability5::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
    }
    else
    {
        if (!abilityReady)
        {
            rightHand->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
        }
        abilityReady = true;
    }
    if (timeToChange > 0.0f)
    {
        timeToChange -= dt;
        if (timeToChange <= 0.0f)
        {
            rightHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
            rightHand->SetLocalPosition({ 0.0f, -2.7f, 1.0f });
        }
    }

}


void Ability5::DrawImGuiControls()
{

    ImGui::InputFloat("Force", &force);
    ImGui::InputFloat("Cooldown", &cooldown);
	ImGui::InputFloat("Duration", &duration);

}