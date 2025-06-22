#include "Throwable.h"
#include <string>
Throwable::Throwable(Node* owner) : Component(owner)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
}

void Throwable::Update(float dt)
{
	velocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
}

void Throwable::OnCollisionEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "TRIGGER") return;
	Vec3 position = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float l = velocity.Length();

	if (pOwner->GetComponent<SoundEffectsPlayer>()) {

		if (velocity.Length() > 20.0f)
		{
			float volumeFactor = std::min(1.0f,
				(velocity.Length() - 20.0f) / (200.0f - 20.0f)
			);

			float gain = std::clamp(volumeFactor, 0.0f, 1.0f);

			// 4. Odtwórz losowy dŸwiêk uderzenia z obliczon¹ g³oœnoœci¹
			int soundIndex = rand() % 5;
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(soundIndex);
		}
	}

	if (object->tag == "ENEMY")
	{
		OutputDebugStringA(("\n\nThrowable hit enemy: " + object->GetName() + " wiht speed: " + std::to_string(l)).c_str());
	}
	if (l < speed) return;

	if (object->tag == "ENEMY" || object->tag == "PLAYER")
	{
		if (object->GetComponent<Health>())
		{
			object->GetComponent<Health>()->TakeDamage(damage, heavy);
		}
	}
	
	if (pot)
	{
		pOwner->Destroy();
	}
}


void Throwable::DrawImGuiControls()
{
	Vec3 position = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float l = position.Length();
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Velocity", &l);

}
