#include "Throwable.h"
#include <string>
Throwable::Throwable(Node* owner) : Component(owner)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
}

void Throwable::OnCollisionEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "TRIGGER") return;
	Vec3 position = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float l = position.Length();
	if (l < speed) return;
	if (object->tag == "ENEMY" || object->tag == "PLAYER")
	{
		if (object->GetComponent<Health>())
		{
			object->GetComponent<Health>()->TakeDamage(damage, heavy);
		}
	}
	if (pOwner->GetComponent<SoundEffectsPlayer>())
	{
		float p = (rand() % 2);
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(p);
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
