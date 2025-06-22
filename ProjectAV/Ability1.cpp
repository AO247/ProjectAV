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
void Ability1::Pressed()
{
    if (!abilityReady) return;
	timeToChange = 0.3f;
	leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
	leftHandNormal->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->tag == "ENEMY" || objects[i]->tag == "STONE")
        {
			Vec3 direction = Vec3(pOwner->Forward().x, pOwner->Forward().y, pOwner->Forward().z);
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force);
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
    }
    cooldownTimer = cooldown;
    abilityReady = false;
    PrefabManager::InstantiateAbility1Particles(pOwner->GetParent(), Vector3(pOwner->GetLocalPosition().x, pOwner->GetLocalPosition().y, pOwner->GetLocalPosition().z), 1.0, pOwner->GetLocalRotationQuaternion());
}
void Ability1::Released()
{
}
void Ability1::Cooldowns(float dt)
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

void Ability1::OnTriggerEnter(Node* object) {
    if (object == nullptr) return;
    if (object->tag != "ENEMY" && object->tag != "STONE" && object->tag != "BULLET") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    for(int i= 0; i < objects.size(); i++)
    {
        if (objects[i] == object) return;
	}
    objects.push_back(object);
	//OutputDebugStringA(("Ability1 OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void Ability1::OnTriggerExit(Node* object) {
    if (object == nullptr) return;
    if (object->tag != "ENEMY" && object->tag != "STONE" && object->tag != "BULLET") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    auto it = std::remove(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        objects.erase(it, objects.end());
    }
	//OutputDebugStringA(("Ability1 OnTriggerExit: " + object->GetName() + "\n").c_str());
}
void Ability1::DrawImGuiControls()
{
	ImGui::InputFloat("Force", &force);
	ImGui::InputFloat("Cooldown", &cooldown);

}