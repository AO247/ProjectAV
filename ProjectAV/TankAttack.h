#pragma once
#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class TankAttack : public Component
{
public:
	TankAttack(Node* owner, std::string tag = "ATTACK");
	virtual ~TankAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 0.92f;
	float startDmgTime = 0.36f;
	float stopDmgTime = 0.68f;
	float knockbackForce = 1500.0f;
	float moveForce = 200.0f;
	float stopMovingTime = 0.9f;
	void OnTriggerEnter(Node* object) override;
	void OnTriggerExit(Node* object) override;

	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;
private:
	void CheckAttack();
	std::vector<Node*> objects;
	bool attacked = false;
	float timer = 0.0f;
};