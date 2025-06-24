#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class FallDamage : public Component
{
public:
	FallDamage(Node* owner, Node* player = nullptr);
	virtual ~FallDamage() = default;
	virtual void Update(float dt) override;
	void OnTriggerStay(const std::vector<Node*> others) override;
	virtual void DrawImGuiControls() override;

	float minFallForce = 6.0f;
	float maxFallForce = 500.0f;
	float maxDistance = 6.0f;
	float fallPushCooldown = 1.0f;
	Vec3 lastVelocity = Vec3(0.0f, 0.0f, 0.0f);

	std::vector<Node*> objects;

private:
	float timer = 0.0f;
	Node* player = nullptr;
	Rigidbody* rigidbody = nullptr;
	PlayerController* playerController = nullptr;
};