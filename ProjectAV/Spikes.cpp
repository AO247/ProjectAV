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
		object->GetComponent<Health>()->TakeDamage(1.0f);
	}
}

void Spikes::DrawImGuiControls()
{

}