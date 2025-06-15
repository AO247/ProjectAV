#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Collectable : public Component
{
public:
	Collectable(Node* owner);
	virtual ~Collectable() = default;
	virtual void DrawImGuiControls() override;
	void OnTriggerEnter(Node* object) override;
	bool health = false;
private:
	Rigidbody* rigidbody;
};
