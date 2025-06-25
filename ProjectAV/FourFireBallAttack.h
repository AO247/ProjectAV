#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class FourFireBallAttack : public Component
{
public:
	FourFireBallAttack(Node* owner, Node* player = nullptr, std::string tag = "ATTACK");
	virtual ~FourFireBallAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 2.36f;
	float knockbackForce = 1500.0f;
	float bulletSpeed = 20.0f;
	float spawnTime1 = 0.80f;
	float spawnTime2 = 1.0f;
	float spawnTime3 = 1.2f;
	float spawnTime4 = 1.4f;

	float shootTime1 = 1.0f;
	float shootTime2 = 1.2f;
	float shootTime3 = 1.4f;
	float shootTime4 = 1.6f;

	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;

	float shootSoundTimer = 0.0f;
	float shootSoundInterval = wholeAttackTime;
	Node* fireBall1 = nullptr;
	Node* fireBall2 = nullptr;
	Node* fireBall3 = nullptr;
	Node* fireBall4 = nullptr;

private:
	bool attacked = false;
	float timer = 0.0f;
	Node* player = nullptr;
};