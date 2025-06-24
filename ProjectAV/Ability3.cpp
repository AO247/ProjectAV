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
		activeSoundTimer -= dt;
        if (timer > 0.0f)
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
                Vec3(0.0f, 0.0f, 0.0f));
            PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
                pos, EActivation::DontActivate);
            

            if (duration - timer > 0.3f)
            {
                // particle ju� aktywowanej
                // d�wi�k ju� aktywowanej
				
                Activated();
            }
            else if (timer == duration)
            {
                if (activeSoundTimer <= 0.0f && pOwner->GetComponent<SoundEffectsPlayer>()) {
                    pOwner->GetComponent<SoundEffectsPlayer>()->Play(2, 1.0f, false);
                    activeSoundTimer = activeSoundInterval;
                }
                // particle po wyl�dowaniu
                // d�wi�k po wyl�dowaniu
                /*if (pOwner->GetComponent<SoundEffectsPlayer>()) {
					pOwner->GetComponent<SoundEffectsPlayer>()->Stop(1);
                    pOwner->GetComponent<SoundEffectsPlayer>()->Play(2, 1.0f, false);
                    activeSoundTimer = activeSoundInterval;
                }*/
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
    }
    if (animTimer > 0.0f)
    {
        animTimer -= dt;
    }
    else if (readyToActive)
    {
        readyToActive = false;
		throwSoundTimer -= dt;

        leftHand->PlayAnimation(1, 0.2f, false); //ATTACK_BLACK_HOLE_END
        rightHand->PlayAnimation(1, 0.2f, false);
        // animacja rzucenia czarnej dziury
        // particle rzucenia czarnej dziury
        // d�wi�k rzucenia czarnej dziury
		if (pOwner->GetComponent<SoundEffectsPlayer>()) {
			if (throwSoundTimer <= 0.0f)
			{
                pOwner->GetComponent<SoundEffectsPlayer>()->Stop(3);
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(1, 1.0f, false);
				throwSoundTimer = throwSoundInterval;
			}
		}

        released = true;
        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(pOwner->GetComponent<Rigidbody>()->GetBodyID(),
            Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z) * 60.0f);
    }
    else if (isPressed)
    {
        leftHand->PlayAnimation(2, 0.2f, false); //ATTACK_BLACK_HOLE_HOLD
        rightHand->PlayAnimation(2, 0.2f, false);

        // animacja trzymania czarnej dziury
        // particle trzymania czarnej dziury
        // d�wi�k trzymania czarnej dziury
        holdSoundTimer -= dt;
        if (pOwner->GetComponent<SoundEffectsPlayer>()) {
            if (holdSoundTimer <= 0.0f)
            {
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(3, 1.0f, false);

                holdSoundTimer = holdSoundInterval;
            }
        }
    }
}
void Ability3::Positioning()
{
    if (isPressed)
    {
        Vector3 cameraPos = camera->GetWorldPosition();
        Vector3 targetPosition = cameraPos + camera->Forward() * 6.0f;
        targetPosition += camera->Down() * 1.0f;
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
    if (killsCount < 3 || isPressed) return false;
    
    Vector3 cameraPos = camera->GetWorldPosition();
    Vector3 targetPosition = cameraPos + camera->Forward() * 6.0f;
    targetPosition += camera->Down() * 1.0f;
    pOwner->SetWorldPosition(targetPosition);
	
    // animacja tworzenia czarnej dziury
    // particle tworzenia czarnej dziury
    // d�wi�k tworzenia czarnej dziury

    leftHand->PlayAnimation(0, 0.2f, false); //ATTACK_BLACK_HOLE 
    rightHand->PlayAnimation(0, 0.2f, false);

    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(0, 1.0f, false);
    }

    animTimer = 1.0f; //czas zakonczenia tworzenia czarnej dziury i przejscie do trzymania
    isPressed = true;
    released = false;
	
    PrefabManager::InstantiateAbility3Particles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    PrefabManager::InstantiateAbility3CoreParticles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    return true;
    //PrefabManager::InstantiateAbility3CoreParticles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
    PrefabManager::InstantiateAbility3CoreSmokeParticles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0);
}
void Ability3::Released()
{
    if (!isPressed || released) return;
    isPressed = false;
    killsCount = 0;
    readyToActive = true;
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