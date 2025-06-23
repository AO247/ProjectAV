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
		holdSoundInterval -= dt;
        if (timer > 0.0f)
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
                Vec3(0.0f, 0.0f, 0.0f));
            PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
                pos, EActivation::DontActivate);
            if (pOwner->GetComponent<SoundEffectsPlayer>()) {
                if (holdSoundTimer <= 0.0f)
                {
                    pOwner->GetComponent<SoundEffectsPlayer>()->Play(3, 1.0f, false);

                    holdSoundTimer = holdSoundInterval;
                }
            }
            if (duration - timer > 0.3f)
            {
                Activated();
            }
            timer -= dt;

        }
        else if (activated)
        {
            activated = false;
            PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
                Vec3(0.0f, 0.0f, 0.0f), EActivation::DontActivate);
        }
        else {
            Positioning();
        }
        Cooldowns(dt);

    }
}
void Ability3::Positioning()
{
    if (isPressed)
    {
        Vector3 cameraPos = camera->GetWorldPosition();
        Vector3 targetPosition = cameraPos + camera->Forward() * 6.0f;
        //pOwner->SetWorldPosition(targetPosition);
        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
            Vec3(targetPosition.x, targetPosition.y, targetPosition.z), EActivation::Activate);
    }
}
void Ability3::OnCollisionEnter(Node* object)
{
    if (!released) return;
    if (object->tag == "ENEMY" || object->tag == "GROUND" || object->tag == "WALL" || object->tag == "STONE")
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
            Vec3(0.0f, 0.0f, 0.0f));
        timer = duration;
        released = false;
        activated = true;
        pos = PhysicsCommon::physicsSystem->GetBodyInterface().GetPosition(pOwner->GetComponent<Rigidbody>()->GetBodyID());
    }

}
bool Ability3::Pressed()
{
    if (killsCount < 3) return false;
    isPressed = true;
    released = false;
	if (pOwner->GetComponent<SoundEffectsPlayer>()) {
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(0, 1.0f, false);
	}
    PrefabManager::InstantiateAbility3Particles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    PrefabManager::InstantiateAbility3CoreParticles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    return true;
}
void Ability3::Released()
{
    if (!isPressed || released) return;
    isPressed = false;
    killsCount = 0;
    released = true;

    PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
        Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z) * 60.0f);
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(1, 1.0f, false);
    }
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
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(2, 1.0f, false);
    }
}

void Ability3::Cooldowns(float dt)
{
}

void Ability3::OnTriggerStay(const std::vector<Node*> others) {
    objects.clear();
    objects = others;
}


void Ability3::DrawImGuiControls()
{
	ImGui::InputFloat("Duration", &duration);
    ImGui::InputFloat("Max Force", &maxForce);
    ImGui::InputFloat("Min Force", &minForce);
    ImGui::InputFloat("Max Distance", &maxDistance);

}