#pragma once
#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "OBB.h"
#include "Components.h"
// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
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
	void OnTriggerEnter(Node* object) override;
	void OnTriggerExit(Node* object) override;
	//OBB* damageArea;

	virtual void Attack(float dt);
	virtual void DrawImGuiControls() override;
private:
	void CheckAttack();
	std::vector<Node*> objects;
	bool attacked = false;
	float timer = 0.0f;
};