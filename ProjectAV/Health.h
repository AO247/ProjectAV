#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Sprite.h"
#include <memory>
class Node;

class Health : public Component
{
public:
	Health(Node* owner, float maxHealth);
	virtual ~Health() = default;
	float maxHealth = 1.0f;
	float currentHealth;
	bool tank = false;
	bool fireType = false;
	bool boss = false;
	std::unique_ptr<Sprite> healthBar1;
	std::unique_ptr<Sprite> healthBar2;

	virtual void DrawImGuiControls() override;
	void TakeDamage(float damage, bool heavy = false, bool isFire = false, bool extraHeavy = false);

private:
};
