#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class BoomAttack : public Component
{
public:
	BoomAttack(Node* owner, Node* player = nullptr, std::string tag = "ATTACK");
	virtual ~BoomAttack() = default;
	virtual void Attack(float dt);
	void OnTriggerStay(const std::vector<Node*> others) override;
	virtual void DrawImGuiControls() override;
	float boomTime = 5.0f;
	float knockRange = 10.0f;
	std::vector<Node*> objects;

	void Boom();
private:
	bool attacked = false;
	float timer = 0.0f;
	Node* player = nullptr;
};