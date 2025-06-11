#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class BrokenPart : public Component
{
public:
	BrokenPart(Node* owner);
	virtual ~BrokenPart() = default;
	virtual void DrawImGuiControls() override;
	void OnCollisionEnter(Node* object) override;
private:
	Rigidbody* rigidbody;
	bool broked = false;
};
