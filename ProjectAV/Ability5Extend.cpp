#include "Ability5Extend.h"
#include <string>
Ability5Extend::Ability5Extend(Node* owner, float force, float duration) 
	: Component(owner), force(force), duration(duration)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Ability5Extend::Update(float dt)
{
    // particle dzia³ania 
    // dŸwiêk dzia³ania
	timer += dt;
	if (timer >= duration) {
		pOwner->Destroy();
	}
    for(int i = 0; i < objects.size(); i++)
    {
        ApplyForce(objects[i], dt);
	}
 
}

void Ability5Extend::ApplyForce(Node* object, float dt) {
    if (object == nullptr || object->GetComponent<Rigidbody>() == nullptr) return;
    //float distance = object->GetWorldPosition().y - pOwner->GetWorldPosition().y;
    //PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(object->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0, 1, 0) * force * (7.0f - distance) * dt);

    float amplitude = 0.6f; // Amplituda oscylacji
    float frequency = 3.0f;  // Czêstotliwoœæ oscylacji (Hz)
    float targetHeight = 13.0f + amplitude * std::sin(timer * frequency * 2.0f * 3.14159265f);

    float currentY = object->GetWorldPosition().y;
    float baseY = pOwner->GetWorldPosition().y;
    float distance = currentY - baseY;
	if (distance < 0) distance = 0; // Zapewnij, ¿e odleg³oœæ nie jest ujemna

    // Pobierz aktualn¹ prêdkoœæ cia³a
    auto* rb = object->GetComponent<Rigidbody>();
    Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rb->GetBodyID());

    float springStrength = force * 10; // Si³a sprê¿yny (przyci¹ganie do wysokoœci)
    //float damping = 50.0f;         // T³umienie (hamowanie ruchu)

    // Si³a sprê¿yny + t³umienie
    float forceY = springStrength * (targetHeight - distance) - damping * velocity.GetY();
    //PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(
    //    rb->GetBodyID(),
    //    Vec3(velocity.GetX() * 0.90f, velocity.GetY(), velocity.GetZ() * 0.90f)
    //);
    if (object->tag == "ENEMY")
    {
		object->GetComponent<StateMachine>()->Stop(0.3f);
    }
    else {
        PhysicsCommon::physicsSystem->GetBodyInterface().SetAngularVelocity(
            rb->GetBodyID(),
            Vec3(0.5f, 0.5f, 0.5f)
		);
    }
    if (object->tag == "BULLET")
    {
        Bullet* bullet = object->GetComponent<Bullet>();
        bullet->pushedByPlayer = true;
        bullet->ignore = nullptr;
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(
            rb->GetBodyID(),
            Vec3(0, 1, 0) * forceY * 0.5f * dt
        );
    }
    else {
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(
            rb->GetBodyID(),
            Vec3(0, 1, 0) * forceY * dt
        );
    }

	holdSoundTimer -= dt;

    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        if (holdSoundTimer <= 0.0f)
        {
            pOwner->GetComponent<SoundEffectsPlayer>()->Play(4);

            holdSoundTimer = holdSoundInterval;
        }
    }
}

void Ability5Extend::OnTriggerEnter(Node* object) {
    if (object == nullptr) return;
    if (object->tag != "ENEMY" && object->tag != "STONE" && object->tag != "BULLET") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    if (object->tag != "ENEMY") 
    {
		Vec3 velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(
            object->GetComponent<Rigidbody>()->GetBodyID()
		);
        PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(
            object->GetComponent<Rigidbody>()->GetBodyID(),
            Vec3(-velocity.GetX(), velocity.GetY(), -velocity.GetZ())
		);
    }
    for (int i = 0; i < objects.size(); i++)
    {
        if (objects[i] == object) return;
    }
    objects.push_back(object);
}

void Ability5Extend::OnTriggerExit(Node* object) {
    if (object == nullptr) return;
    if (object->tag != "ENEMY" && object->tag != "STONE" && object->tag != "BULLET") return;
    if (object->GetComponent<Rigidbody>() == nullptr) return;
    auto it = std::remove(objects.begin(), objects.end(), object);
    if (it != objects.end()) {
        objects.erase(it, objects.end());
    }
}

void Ability5Extend::DrawImGuiControls()
{
	ImGui::InputFloat("Force", &force);
	ImGui::InputFloat("Damping", &damping);


}
