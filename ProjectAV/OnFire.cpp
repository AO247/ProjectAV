#include "OnFire.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
OnFire::OnFire(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void OnFire::Update(float dt)
{
	timer += dt;
	if (pOwner->tag == "PLAYER")
	{
		if (timer >= 1.5f && !attacked)
		{
			pOwner->GetComponent<Health>()->TakeDamage(1.0f, false, true);
			attacked = true;
		}
		else if (timer >= 3.0f)
		{
			pOwner->RemoveComponent(this);
		}
	}
	else
	{
		if (timer >= fireRate)
		{
			pOwner->GetComponent<Health>()->TakeDamage(damage, false, true);
			timer = 0.0f;
		}

	}
}
