#include "FireBoom.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
FireBoom::FireBoom(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void FireBoom::Update(float dt)
{
	timer += dt;
	if (timer > duration)
	{
		pOwner->Destroy();
	}
}


void FireBoom::OnTriggerStay(const std::vector<Node*> others)
{
	for (int i = 0; i < others.size(); ++i)
	{
		if (others[i] == nullptr) return;
		if (others[i]->tag == "PLAYER" || others[i]->tag == "ENEMY")
		{
			if (others[i]->GetComponent<OnFire>() != nullptr) return;
			others[i]->AddComponent(
				std::make_unique<OnFire>(others[i])
			);
		}
	}

}


void FireBoom::DrawImGuiControls()
{
}