#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class FireBallAttack : public Component
{
public:
	FireBallAttack(Node* owner, Node* player = nullptr, std::string tag = "ATTACK");
	virtual ~FireBallAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 2.04f;
	float knockbackForce = 1500.0f;
	float bulletSpeed = 20.0f;
	float spawnTime = 0.4f;
	float shootTime = 1.0f;
	bool spawned = false;
	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;

	float shootSoundTimer = 0.0f;
	float shootSoundInterval = wholeAttackTime;
	Node* fireBall = nullptr;
private:
	bool attacked = false;
	float timer = 0.0f;
	Node* player = nullptr;
};