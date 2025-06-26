#include "OnFire.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
OnFire::OnFire(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	if (owner->GetComponent<PlayerController>() != nullptr)
	{
		particles = PrefabManager::InstantiatePlayerOnFireParticles(pOwner, Vector3(0, 0, 0), 1.0f, 3.0f);
	}
	else
	{
		particles = PrefabManager::InstantiateCharacterOnFireParticles(pOwner, Vector3(0, 0, 0), 1.0f, 3.0f);
	}
	
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
			if (particles != nullptr)
			{
				particles->GetComponent<ParticleSystemComponent>()->Stop();
			}
			pOwner->RemoveComponent(this);
		}
	}
	else
	{
		if (timer >= fireRate)
		{
			/*if (particles != nullptr)
			{
				particles->GetComponent<ParticleSystemComponent>()->Stop();
			}*/
			pOwner->GetComponent<Health>()->TakeDamage(damage, false, true);
			timer = 0.0f;
		}

	}
}
