#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
class Node;

class Health : public Component
{
public:
	Health(Node* owner, float maxHealth);
	virtual ~Health() = default;
	float maxHealth = 1.0f;
	float currentHealth;
	virtual void DrawImGuiControls() override;
	void TakeDamage(float damage);

private:
};
