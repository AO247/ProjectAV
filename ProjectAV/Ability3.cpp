/////////////////////////
// Evolve of ability 2 //
// BLACK HOLE          //
/////////////////////////
#include "Ability3.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm>
#include <string>
#include "PrefabManager.h"
#include "Components.h"

namespace dx = DirectX;
Ability3::Ability3(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{
}


void Ability3::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        if (timer > 0.0f)
        {
            Activated();
            timer -= dt;
            if (timer <= 0.0f)
            {
                timer = 0.0f;
            }

        }
        else {
            Positioning();
        }
        Cooldowns(dt);
    }
}
void Ability3::Positioning()
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
void Ability3::Pressed()
{
    if (killsCount < 3) return;
    killsCount = 0;
    timer = duration;
    cooldownTimer = cooldown;
    abilityReady = false;
    PrefabManager::InstantiateAbility3Particles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    PrefabManager::InstantiateAbility3CoreParticles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
}
void Ability3::Released()
{
}
void Ability3::Activated()
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i] != nullptr)
        {
            if (objects[i]->tag == "ENEMY" || objects[i]->tag == "STONE")
            {
                Vector3 objPos = objects[i]->GetWorldPosition();
                Vector3 aPos = pOwner->GetWorldPosition();
                aPos.y += 1.0f;
                Vector3 direction = aPos - objPos;

                float distance = direction.Length();


                distance = std::clamp(distance, 0.1f, maxDistance);

                float scaledForce = maxForce * (1.0f - (distance / maxDistance));
                scaledForce = std::max(scaledForce, minForce);
                direction.Normalize();

                PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
                PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3(direction.x, direction.y, direction.z) * scaledForce);
                //OutputDebugStringA(("Ability3 hit: " + objects[i]->GetName() + "\n").c_str());
            }
            else if (objects[i]->tag == "BULLET")
            {
                Vec3 direction = Vec3(pOwner->Forward().x, pOwner->Forward().y, pOwner->Forward().z);
                Bullet* bullet = objects[i]->GetComponent<Bullet>();
                bullet->pushedByPlayer = true;
                bullet->ignore = nullptr;
                PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force * 0.04f);
            }
        }
    }
}

void Ability3::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
    }
    else
    {
        abilityReady = true;
    }

}

void Ability3::OnTriggerStay(const std::vector<Node*> others) {
    objects.clear();
    objects = others;
}


void Ability3::DrawImGuiControls()
{
    ImGui::InputFloat("Cooldown", &cooldown);
	ImGui::InputFloat("Duration", &duration);
    ImGui::InputFloat("Max Force", &maxForce);
    ImGui::InputFloat("Min Force", &minForce);
    ImGui::InputFloat("Max Distance", &maxDistance);

}