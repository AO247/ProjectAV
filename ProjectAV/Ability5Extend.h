#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Ability5Extend : public Component
{
public:
	Ability5Extend(Node* owner, float force, float duration);
	virtual ~Ability5Extend() = default;
	virtual void Update(float dt) override;
	float damage = 1.0f;
	float duration = 3.0f;
	float force = 100.0f;
	float damping = 1000.0f;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnTriggerExit(Node* other) override;
	Node* ignore = nullptr;
	std::vector<Node*> objects;

	float holdSoundTimer = 0.0f;
	float holdSoundInterval = 2.0f;
private:
	Rigidbody* rigidbody;
	void ApplyForce(Node* other, float dt);
	float timer = 0.0f;

};
