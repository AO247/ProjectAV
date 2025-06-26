#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class OnFire : public Component
{
public:
	OnFire(Node* owner, std::string tag = "FIRE");
	virtual ~OnFire() = default;
	float fireRate = 0.5f;
	float damage = 0.1f;
	float bigTimer = 0.0f;
	virtual void Update(float dt) override;

private:
	bool attacked = false;
	float timer = 0.0f;
	Node* particles = nullptr;
};