///////////////////////////
// Classic throw ability //
///////////////////////////
#include "Ability2.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm>
#include <string>
#include "PrefabManager.h"
#include "ParticleSystemComponent.h"

#include "Components.h"
namespace dx = DirectX;
Ability2::Ability2(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{

}


void Ability2::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        Positioning();
        Cooldowns(dt);
    }
}
void Ability2::Positioning()
{
	Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
	Vec3 direction = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
    RRayCast ray = RRayCast(position, direction * 100.0f);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result, 
        IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER}), 
        IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER})))
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
bool Ability2::Pressed()
{
    if (!abilityReady) return false;

    // animacja
    // particle

    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        float randSound = (rand() % 4);
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
    }
    //rightHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
    //rightHand->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });

    rightHand->PlayAnimation(7, 0.2, false); //TOSS
    timeToChange = 0.3f;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->tag == "ENEMY")
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
            Vec3 direction = Vec3(0.0f, 1.0f, 0.0f);
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force);
            objects[i]->GetComponent<StateMachine>()->Stop(0.8f);
        }
        else if (objects[i]->tag == "STONE")
        {
            Vec3 direction = Vec3(0.0f, 1.0f, 0.0f);
            PhysicsCommon::physicsSystem->GetBodyInterface().SetAngularVelocity(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3Arg(1.0f, 1.0f, -1.0f));
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * 230.0f);
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
    cooldownTimer = cooldown;
    abilityReady = false;
    PrefabManager::InstantiateAbility2Particles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    PrefabManager::InstantiateAbility2ParticlesSmoke(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    //PrefabManager::InstantiateExpCollectable(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 0.7, pOwner->GetLocalPosition().y);

    return true;
}
void Ability2::Released()
{
}

void Ability2::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
        if (rightHand->GetCurrentPlayingAnimationRaw() == nullptr) {
            rightHand->PlayAnimation(8); //COOLDOWN
        }
    }
    else
    {
        if (!abilityReady)
        {
            rightHand->PlayAnimation(13); //IDLE RUN
        }
        abilityReady = true;
    }
    if (timeToChange > 0.0f)
    {
        timeToChange -= dt;
        if (timeToChange <= 0.0f)
        {
            //rightHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
            //rightHand->SetLocalPosition({ 0.0f, -2.7f, 1.0f });
        }
    }

}


void Ability2::OnTriggerStay(const std::vector<Node*> others) {
	objects.clear();
    objects = others;
}

void Ability2::DrawImGuiControls()
{

    ImGui::InputFloat("Force", &force);
    ImGui::InputFloat("Cooldown", &cooldown);

}