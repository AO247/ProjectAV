#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Bullet : public Component
{
public:
	Bullet(Node* owner);
	virtual ~Bullet() = default;
	virtual void Update(float dt) override;
	float damage = 1.0f;
	float lifeTime = 5.0f;
	float knockbackForce = 1500.0f;
	//BoundingSphere* damageArea;
	virtual void DrawImGuiControls() override;
	void OnCollisionEnter(Node* other) override;
	void OnTriggerEnter(Node* other) override;
	Node* ignore = nullptr;

private:
	Rigidbody* rigidbody;
	//void OnTriggerEnter();
	float timer = 0.0f;

};
