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
	if (timer >= fireRate)
	{
		pOwner->GetComponent<Health>()->TakeDamage(damage, false, true);
		timer = 0.0f;
	}
}
