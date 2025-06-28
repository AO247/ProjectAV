#include "FireBall.h"
#include <string>
#include "PrefabManager.h"
FireBall::FireBall(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
	// particle
	PrefabManager::InstantiateFireballParticles(pOwner, Vector3(0, 0, 0), 1.0f);
}
void FireBall::Update(float dt)
{
	timer += dt;
	if (timer >= lifeTime) {
		pOwner->Destroy();
	}
}

void FireBall::OnTriggerEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "TRIGGER" || object == pOwner || object == ignore) return;
	if (object->tag == "PLAYER" || object->tag == "GROUND" || object->tag == "WALL" || object->tag == "STONE")
	{
		if (object->tag == "PLAYER" || object->tag == "ENEMY")
		{
			if (object->GetComponent<OnFire>() != nullptr) return;
			object->AddComponent(
				std::make_unique<OnFire>(object)
			);
		}
		//PrefabManager::InstantiateFireBoom(pOwner->GetWorldPosition(), 1.0f);
		pOwner->Destroy();

	}
	if (pushedByPlayer && object->tag == "ENEMY")
	{
		if (object->tag == "PLAYER" || object->tag == "ENEMY")
		{
			if (object->GetComponent<OnFire>() != nullptr) return;
			object->AddComponent(
				std::make_unique<OnFire>(object)
			);
		}
		//PrefabManager::InstantiateFireBoom(pOwner->GetWorldPosition(), 1.0f);
		object->GetComponent<Health>()->TakeDamage(0.0f);
		pOwner->Destroy();
	}
}
void FireBall::OnCollisionEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "TRIGGER" || object == pOwner || object == ignore) return;
	//PrefabManager::InstantiateFireBoom(pOwner->GetWorldPosition(), 1.0f);
	if (object->tag == "PLAYER" || object->tag == "ENEMY")
	{
		if (object->GetComponent<OnFire>() != nullptr) return;
		object->AddComponent(
			std::make_unique<OnFire>(object)
		);
	}
	pOwner->Destroy();
}

void FireBall::DrawImGuiControls()
{

}
