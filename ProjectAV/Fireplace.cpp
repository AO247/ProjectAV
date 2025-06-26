#include "Fireplace.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
Fireplace::Fireplace(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void Fireplace::Update(float dt)
{
	timer += dt;
	if (isActive)
	{
		//fireSoundTimer = -dt;
		if (timer >= activeTime)
		{
			timer = 0.0f;
			isActive = false;
			if (particles != nullptr)
			{
				particles->GetComponent<ParticleSystemComponent>()->Stop();
				particles = nullptr;
			}
		}
	}
	else
	{
		if (timer >= cooldownTime)
		{
			timer = 0.0f;
			isActive = true;
			particles = PrefabManager::InstantiateFireplaceParticles(pOwner, Vector3(0, 0, 0), 1.0f, activeTime);
		}
	}
}


void Fireplace::OnTriggerStay(const std::vector<Node*> others)
{
	for(int i = 0; i < others.size(); ++i)
	{
		if (others[i] == nullptr) return;
		if (!isActive) return;
		if (others[i]->tag == "PLAYER" || others[i]->tag == "ENEMY")
		{
			if (others[i]->GetComponent<OnFire>() != nullptr) return;
			others[i]->AddComponent(
				std::make_unique<OnFire>(others[i])
			);
		}
	}
	
}


void Fireplace::DrawImGuiControls()
{
}