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
		if (timer >= activeTime)
		{
			timer = 0.0f;
			isActive = false;
		}
	}
	else
	{
		if (timer >= cooldownTime)
		{
			timer = 0.0f;
			isActive = true;
		}
	}
}


void Fireplace::OnTriggerStay(Node* object)
{
	if (!isActive) return;
	if (object->tag == "PLAYER" || object->tag == "ENEMY")
	{
		if (object->GetComponent<OnFire>() != nullptr) return;
		object->AddComponent(
			std::make_unique<OnFire>(object)
		);
	}
}


void Fireplace::DrawImGuiControls()
{
}