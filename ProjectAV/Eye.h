#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Eye : public Component
{
public:
	Eye(Node* owner);
	virtual ~Eye() = default;
	virtual void Update(float dt) override;
	float damage = 1.0f;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* other) override;
	Node* ignore = nullptr;
private:
	Rigidbody* rigidbody;
	float timer = 0.0f;

};
