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

		if (velocity.Length() > minSoundSpeed)
		{
			float volumeFactor = std::min(1.0f,
				(velocity.Length() - minSoundSpeed) / (maxSoundSpeed - minSoundSpeed)
			);

			float gain = std::clamp(volumeFactor, 0.0f, 1.0f);

			int randSound = rand() % 5;
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound);
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
			object->GetComponent<Health>()->TakeDamage(damage, heavy, false, extraHeavy);
		}
	}
	
	if (pot)
	{
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(0);
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
