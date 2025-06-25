#include "Spikes.h"
#include "Node.h"       


namespace dx = DirectX;
Spikes::Spikes(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}


void Spikes::OnCollisionEnter(Node* object)
{
	if (object->tag == "ENEMY" || object->tag == "PLAYER")
	{
		if(PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(object->GetComponent<Rigidbody>()->GetBodyID()).Length() > 30.0f)
			object->GetComponent<Health>()->TakeDamage(1.0f);
	}
}

void Spikes::DrawImGuiControls()
{

}