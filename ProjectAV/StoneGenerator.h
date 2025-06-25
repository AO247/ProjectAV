#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
class Node;

class StoneGenerator : public Component
{
public:
	StoneGenerator(Node* owner);
	virtual ~StoneGenerator() = default;
	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	float dropRate = 10.0f;
	float timer = 0.0f;
	bool active = false;
private:
};
