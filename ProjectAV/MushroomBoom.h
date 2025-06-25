#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class MushroomBoom : public Component
{
public:
	MushroomBoom(Node* owner, std::string tag = "FIRE");
	virtual ~MushroomBoom() = default;
	virtual void Update(float dt) override;
	void OnTriggerStay(const std::vector<Node*> others) override;
	void OnTriggerEnter(Node* object) override;
	virtual void DrawImGuiControls() override;
	float activeTime = 3.0f;
	float cooldownTime = 5.0f;
	float cooldownTimer = 0.0f;
	bool isActive = false;
private:
	float timer = 0.0f;
};