#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Throwable : public Component
{
public:
	Throwable(Node* owner);
	virtual ~Throwable() = default;
	float damage = 1.0f;
	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnCollisionEnter(Node* object) override;
	float speed = 30.0f;
	bool pot = false;
	bool heavy = false;
	Vec3 velocity = Vec3(0.0f, 0.0f, 0.0f);
private:
	Rigidbody* rigidbody;
};
