#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class ShootAttack : public Component
{
public:
	ShootAttack(Node* owner, std::string tag = "Attack");
	virtual ~ShootAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 1.5f;
	float shootTime = 0.5f;
	float knockbackForce = 1500.0f;

	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;

private:
	bool attacked = false;
	float timer = 0.0f;
};