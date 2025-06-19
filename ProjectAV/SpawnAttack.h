#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class SpawnAttack : public Component
{
public:
	SpawnAttack(Node* owner, Node* player = nullptr, std::string tag = "ATTACK");
	virtual ~SpawnAttack() = default;
	float cooldownTime = 3.0f;
	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;

private:
	bool attacked = false;
	float timer = 0.0f;
	Node* player = nullptr;
};