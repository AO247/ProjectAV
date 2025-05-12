#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Collider.h"
#include "BoundingSphere.h"
class Node;

class Throwable : public Component
{
public:
	Throwable(Node* owner);
	virtual ~Throwable() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	BoundingSphere* damageArea;
private:
	Rigidbody* rigidbody;
	void OnTriggerEnter();
};
