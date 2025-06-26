#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class Fireplace : public Component
{
public:
	Fireplace(Node* owner, std::string tag = "FIRE");
	virtual ~Fireplace() = default;
	virtual void Update(float dt) override;
	void OnTriggerStay(const std::vector<Node*> others) override;
	virtual void DrawImGuiControls() override;
	float activeTime = 5.0f;
	float cooldownTime = 5.0f;
	bool isActive = false;
private:
	float timer = 0.0f;
	Node* particles = nullptr;
};