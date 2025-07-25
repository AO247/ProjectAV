//////////////////////////
// Classic push ability //
//////////////////////////
#include "Ability1.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm>
#include <string>
#include "PrefabManager.h"
#include "Components.h"

namespace dx = DirectX;
Ability1::Ability1(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{

}


void Ability1::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        Positioning();
        Cooldowns(dt);
    }
}
void Ability1::Positioning() {
	pOwner->SetLocalTransform(camera->GetLocalTransform());
	pOwner->TranslateLocal(Vector3(0.0f, 0.0f, 8.0f));
}
bool Ability1::Pressed()
{
    if (!abilityReady) return false;

    // animacja popchnięcia 
    // particle

    leftHand->PlayAnimation(6, 0.2, false); //ATTACK_PUSH_LEFT
    
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        float randSound = (rand() % 4);
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
    }

	timeToChange = 0.64f;
	//leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
	//leftHand->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->tag == "ENEMY" || objects[i]->tag == "STONE")
        {
			Vec3 direction = Vec3(pOwner->Forward().x, pOwner->Forward().y, pOwner->Forward().z);
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force);
            if (objects[i]->tag == "ENEMY")
            {
                objects[i]->GetComponent<StateMachine>()->Stop(0.1f);
            }
          /*  if (objects[i]->tag == "ENEMY")
            {
                objects[i]->AddComponent(
                    std::make_unique<OnFire>(objects[i])
                );
            }*/
        }
        else if (objects[i]->tag == "BULLET")
        {
            Vec3 direction = Vec3(pOwner->Forward().x, pOwner->Forward().y, pOwner->Forward().z);
            Bullet* bullet = objects[i]->GetComponent<Bullet>();
            bullet->pushedByPlayer = true;
            bullet->ignore = nullptr;
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force * 0.04f);
        }
        else if (objects[i]->tag == "FIREBALL")
        {
            Vec3 direction = Vec3(pOwner->Forward().x, pOwner->Forward().y, pOwner->Forward().z);
            FireBall* bullet = objects[i]->GetComponent<FireBall>();
            bullet->pushedByPlayer = true;
            bullet->ignore = nullptr;
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force * 0.04f);
        }
    }
    cooldownTimer = cooldown;
    abilityReady = false;
    
    PrefabManager::InstantiateAbility1ParticlesSmoke(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0, pOwner->GetLocalRotationQuaternion());
    return true;
}
void Ability1::Released()
{
}
void Ability1::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
        if (leftHand->GetCurrentPlayingAnimationRaw() == nullptr) {
            leftHand->PlayAnimation(8);  //COOLDOWN
        }
        

    }
    else
    {
        if (!abilityReady)
        {
            leftHand->PlayAnimation(13); //IDLE_RUN
        }
        abilityReady = true;
    }
    if (timeToChange > 0.0f)
    {
        timeToChange -= dt;
        if (timeToChange <= 0.0f)
        {
        }
    }

}

void Ability1::OnTriggerStay(const std::vector<Node*> others) {
    objects.clear();
	objects = others;
    /*if (object == nullptr) return;
    if (object->tag != "ENEMY" && object->tag != "STONE" && object->tag != "BULLET") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i] == object) return;
    }
    objects.push_back(object);*/
}
void Ability1::DrawImGuiControls()
{
	ImGui::InputFloat("Force", &force);
	ImGui::InputFloat("Cooldown", &cooldown);

}