#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class FireBall : public Component
{
public:
	FireBall(Node* owner);
	virtual ~FireBall() = default;
	virtual void Update(float dt) override;
	float damage = 1.0f;
	float lifeTime = 5.0f;
	float knockbackForce = 1500.0f;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	void OnCollisionEnter(Node* other) override;
	Node* ignore = nullptr;
	bool pushedByPlayer = false;
	void Hit();
private:
	Rigidbody* rigidbody;
	float timer = 0.0f;

};
