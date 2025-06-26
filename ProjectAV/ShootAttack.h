#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class ShootAttack : public Component
{
public:
	ShootAttack(Node* owner, Node* player = nullptr, std::string tag = "ATTACK");
	virtual ~ShootAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 1.34f;
	float shootTime = 0.5f;
	float knockbackForce = 1500.0f;
	float bulletSpeed = 20.0f;
	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;

	float shootSoundTimer = 0.0f;
	float shootSoundInterval = wholeAttackTime;
private:
	bool attacked = false;
	float timer = 0.0f;
	Node* player = nullptr;
};