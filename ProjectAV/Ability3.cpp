#include "Ability3.h"
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
Ability3::Ability3(Node* owner, Window& window, Node* camera)
    : Component(owner), wnd(window), camera(camera)  // Initialize reference member
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
            RRayCast ray2 = RRayCast(Vec3(body->GetWorldPosition().x, body->GetWorldPosition().y, body->GetWorldPosition().z), Vec3(0.0f, -100.0f, 0.0f));
            RayCastResult result2;
            if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray2, result2, SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND), SpecifiedObjectLayerFilter(Layers::GROUND)))
            {
                position = ray2.mOrigin + ray2.mDirection * result2.mFraction;
                pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
            }
        }
        else
        {
            position = ray.mOrigin + ray.mDirection * result.mFraction;
            pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
        }
    }
}
void Ability3::Active()
{
    if (!abilityReady) return;
    timer = duration;
    cooldownTimer = cooldown;
    abilityReady = false;
}
void Ability3::Activated()
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i]->tag == "ENEMY" || objects[i]->tag == "STONE")
        {
            Vector3 objPos = objects[i]->GetWorldPosition();
            Vector3 aPos = pOwner->GetWorldPosition();
			aPos.y += 1.0f; // Adjust the height of the ability effect
            Vector3 direction = aPos - objPos;

            float distance = direction.Length();
           

            distance = std::clamp(distance, 0.1f, maxDistance);

            float scaledForce = maxForce * (1.0f - (distance / maxDistance));
            scaledForce = std::max(scaledForce, minForce);
            direction.Normalize(); // Ensure direction is a unit vector

            PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
            PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(), Vec3(direction.x, direction.y, direction.z) * scaledForce);
            OutputDebugStringA(("Ability3 hit: " + objects[i]->GetName() + "\n").c_str());
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


void Ability3::KeyboardInput()
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


void Ability3::OnTriggerEnter(Node* object) {
    if (object->tag != "ENEMY" && object->tag != "STONE") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i] == object) return;
    }
    objects.push_back(object);
    OutputDebugStringA(("Ability2 OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void Ability3::OnTriggerExit(Node* object) {
    if (object->tag != "ENEMY" && object->tag != "STONE") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    auto it = std::remove(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        objects.erase(it, objects.end());
    }
    OutputDebugStringA(("Ability2 OnTriggerExit: " + object->GetName() + "\n").c_str());
}

void Ability3::DrawImGuiControls()
{
    ImGui::InputFloat("Cooldown", &cooldown);
	ImGui::InputFloat("Duration", &duration);
    ImGui::InputFloat("Max Force", &maxForce);
    ImGui::InputFloat("Min Force", &minForce);
    ImGui::InputFloat("Max Distance", &maxDistance);

}