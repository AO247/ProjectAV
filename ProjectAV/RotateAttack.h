#pragma once
#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class RotateAttack : public Component
{
public:
	RotateAttack(Node* owner, Node* playerNode, std::string tag = "ATTACK");
	virtual ~RotateAttack() = default;
	float damage = 1.0f;
	float wholeAttackTime = 7.5f;
	float startDmgTime = 0.88f;
	float stopDmgTime = 6.24f;
	float knockbackForce = 1500.0f;
	float moveForce = 10.0f;
	float startMovingTime = 0.4f;
	float stopMovingTime = 7.0f;
	float rotationSpeed = 2.0f;
	void OnTriggerEnter(Node* object) override;
	void OnTriggerExit(Node* object) override;

	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;
private:
	void CheckAttack();
	std::vector<Node*> objects;
	bool attacked = false;
	float timer = 0.0f;
	Node* player = nullptr;

};