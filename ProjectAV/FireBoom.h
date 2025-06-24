#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class FireBoom : public Component
{
public:
	FireBoom(Node* owner, std::string tag = "FIRE");
	virtual ~FireBoom() = default;
	virtual void Update(float dt) override;
	void OnTriggerStay(const std::vector<Node*> others) override;
	virtual void DrawImGuiControls() override;
	float duration = 5.0f;
private:
	float timer = 0.0f;
};