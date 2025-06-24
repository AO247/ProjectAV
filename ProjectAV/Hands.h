#pragma once

#include "Component.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "Components.h"
class Node;

class Hands : public Component
{
public:
	Hands(Node* owner);
	virtual ~Hands() = default;
	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	float radius = 0.5f;
	Rigidbody* rigidbody = nullptr;
	PlayerController* playerController = nullptr;
	Node* leftHand = nullptr;
	Node* rightHand = nullptr;
	Node* cameraNode = nullptr;
private:
};
