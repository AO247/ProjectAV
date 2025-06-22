#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Collectable : public Component
{
public:
	Collectable(Node* owner, float targetY);
	virtual ~Collectable() = default;
	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* object) override;
	Vector3 basePosition;
	bool health = false;
	float speed = 0.0f;
	float timer = 0.0f;
	float targetY;
	int dirx, dirz;
private:
	Rigidbody* rigidbody;
};
