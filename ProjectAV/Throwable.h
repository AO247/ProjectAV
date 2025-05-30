#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Collider.h"
#include "BoundingSphere.h"
#include "Components.h"
class Node;

class Throwable : public Component
{
public:
	Throwable(Node* owner);
	virtual ~Throwable() = default;
	float damage = 1.0f;
	virtual void DrawImGuiControls() override;
	void OnCollisionEnter(Node* object) override;
	//BoundingSphere* damageArea;
private:
	Rigidbody* rigidbody;
};
