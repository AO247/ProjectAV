#include "Ability2.h"
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
Ability2::Ability2(Node* owner, Window& window, Node* camera)
    : Component(owner), wnd(window), camera(camera)  // Initialize reference member
{

}


void Ability2::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        Positioning();
        Cooldowns(dt);
        //KeyboardInput();
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
            RRayCast ray2 = RRayCast(Vec3(body->GetWorldPosition().x, body->GetWorldPosition().y, body->GetWorldPosition().z), Vec3(0.0f, -100.0f, 0.0f));
            RayCastResult result2;
            if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray2, result2, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
            {
                position = ray2.mOrigin + ray2.mDirection * result2.mFraction;;
            }
        }
        else 
        {
            position = ray.mOrigin + ray.mDirection * result.mFraction;
        }
		pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
    }
}
void Ability2::Active()
{
    if (!abilityReady) return;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->tag == "Enemy" || objects[i]->tag == "Stone")
        {
            //PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
            Vec3 direction = Vec3(0.0f, 1.0f, 0.0f);
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), direction * force * 1.0f);
            OutputDebugStringA(("Ability2 hit: " + objects[i]->GetName() + "\n").c_str());
        }
    }
    cooldownTimer = cooldown;
    abilityReady = false;
}

void Ability2::Cooldowns(float dt)
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


void Ability2::KeyboardInput()
{
    while (const auto e = wnd.mouse.Read()) // Read events from the queue
    {
        switch (e->GetType())
        {
        case Mouse::Event::Type::RPress:
			OutputDebugStringA("\n\n\nRight Mouse Button Pressed\n");
            Active();
            break;
        }
    }
}


void Ability2::OnTriggerEnter(Node* object) {
    if (object->tag != "Enemy" && object->tag != "Stone") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i] == object) return;
    }
    objects.push_back(object);
    OutputDebugStringA(("Ability2 OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void Ability2::OnTriggerExit(Node* object) {
    if (object->tag != "Enemy" && object->tag != "Stone") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    auto it = std::remove(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        objects.erase(it, objects.end());
    }
    OutputDebugStringA(("Ability2 OnTriggerExit: " + object->GetName() + "\n").c_str());
}

void Ability2::DrawImGuiControls()
{

    ImGui::InputFloat("Force", &force);
    ImGui::InputFloat("Cooldown", &cooldown);

}