#pragma once
#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class BasicAttack : public Component
{
public:
	BasicAttack(Node* owner, std::string tag = "ATTACK");
	virtual ~BasicAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 1.5f;
	float startDmgTime = 0.5f;
	float stopDmgTime = 1.0f;
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