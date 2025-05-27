#include "Ability1.h"
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
Ability1::Ability1(Node* owner, Window& window, Node* camera)
    : Component(owner), wnd(window), camera(camera)  // Initialize reference member
{

}


void Ability1::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        Positioning();
        Cooldowns(dt);
        //KeyboardInput();
    }
}
void Ability1::Positioning() {
	pOwner->SetLocalTransform(camera->GetLocalTransform());
	pOwner->TranslateLocal(Vector3(0.0f, 0.0f, 6.0f));
}
void Ability1::Active()
{
    if (!abilityReady) return;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->tag == "Enemy" || objects[i]->tag == "Stone")
        {
			Vec3 direction = Vec3(pOwner->Forward().x, pOwner->Forward().y, pOwner->Forward().z);
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force);
			OutputDebugStringA(("Ability1 hit: " + objects[i]->GetName() + "\n").c_str());
        }
    }
    cooldownTimer = cooldown;
    abilityReady = false;
}

void Ability1::Cooldowns(float dt)
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


void Ability1::KeyboardInput()
{
    while (const auto e = wnd.mouse.Read()) // Read events from the queue
    {
        switch (e->GetType())
        {
        case Mouse::Event::Type::LPress:
            OutputDebugStringA("\n\n\nLeft Mouse Button Pressed\n");
            Active();
            break;
        }
    }
}
void Ability1::OnTriggerEnter(Node* object) {
    if (object->tag != "Enemy" && object->tag != "Stone") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    for(int i= 0; i < objects.size(); i++)
    {
        if (objects[i] == object) return;
	}
    objects.push_back(object);
	OutputDebugStringA(("Ability1 OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void Ability1::OnTriggerExit(Node* object) {
    if (object->tag != "Enemy" && object->tag != "Stone") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    auto it = std::remove(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        objects.erase(it, objects.end());
    }
	OutputDebugStringA(("Ability1 OnTriggerExit: " + object->GetName() + "\n").c_str());
}
void Ability1::DrawImGuiControls()
{
	ImGui::InputFloat("Force", &force);
	ImGui::InputFloat("Cooldown", &cooldown);

}