#include "MushroomBoom.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
MushroomBoom::MushroomBoom(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void MushroomBoom::Update(float dt)
{
	if (isActive)
	{
		if (timer <= 0.0f)
		{
			smokeParticlesSpawned = false;
			//d�wi�k wybuchu
			if (!boomParticleSpawned)
			{
				PrefabManager::InstantiateMushroomExplosionParticles(pOwner->GetParent(), Vector3(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z), 1.0f);
				boomParticleSpawned = true;
			}
		}
		else if (timer > 0.1f)
		{
			if (!smokeParticlesSpawned)
			{
				PrefabManager::InstantiateMushroomSmokeParticles(pOwner->GetParent(), Vector3(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z), 1.0f, activeTime);
				PrefabManager::InstantiateMushroomParticles(pOwner->GetParent(), Vector3(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z), 1.0f, activeTime);
				smokeParticlesSpawned = true;
			}
		}
		if (timer >= activeTime)
		{
			timer = 0.0f;
			isActive = false;
			boomParticleSpawned = false;
			PhysicsCommon::physicsSystem->GetBodyInterface().SetShape(pOwner->GetComponent<Trigger>()->GetBodyID(),
				new JPH::SphereShape(5.0f), false, EActivation::Activate);
			cooldownTimer = cooldownTime;
		}
	}
	if (cooldownTimer > 0.0f && !isActive)
	{
		cooldownTimer -= dt;
	}
	timer += dt;
}


void MushroomBoom::OnTriggerStay(const std::vector<Node*> others)
{
	for (int i = 0; i < others.size(); ++i)
	{
		if (others[i] == nullptr) return;
		if (!isActive) return;
		if (others[i]->tag == "PLAYER")
		{
			others[i]->GetComponent<PlayerController>()->slowTime = 0.5f;
		}
		else if (others[i]->tag == "ENEMY")
		{
			others[i]->GetComponent<StateMachine>()->Stun(0.3f);
		}
	}

}
void MushroomBoom::OnTriggerEnter(Node* object)
{
	if (cooldownTimer > 0.0f) return;
	if (object->tag == "PLAYER" || object->tag == "ENEMY" || object->tag == "STONE")
	{
		isActive = true;
		timer = 0.0f;
		PhysicsCommon::physicsSystem->GetBodyInterface().SetShape(pOwner->GetComponent<Trigger>()->GetBodyID(),
			new JPH::SphereShape(20.0f), false, EActivation::Activate);
	}
}


void MushroomBoom::DrawImGuiControls()
{
}