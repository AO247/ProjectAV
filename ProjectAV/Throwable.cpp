#include "Throwable.h"
#include <string>
Throwable::Throwable(Node* owner) : Component(owner)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
}

void Throwable::OnCollisionEnter(Node* object)
{
	Vec3 position = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float l = position.Length();
	if (l < 35.0f) return;
	if (object->GetComponent<Health>())
	{
		object->GetComponent<Health>()->TakeDamage(damage);
	}
	if (!pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying())
	{
		float p = (rand() % 2);
		pOwner->GetComponent<SoundEffectsPlayer>()->Play(p);
	}
}


void Throwable::DrawImGuiControls()
{
	Vec3 position = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float l = position.Length();
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Velocity", &l);

}
