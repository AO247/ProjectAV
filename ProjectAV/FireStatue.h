#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class FireStatue : public Component
{
public:
	FireStatue(Node* owner, std::string tag = "WALL");
	virtual ~FireStatue() = default;
	virtual void Update(float dt) override;
	float spinSpeed = 1.0f;
private:
};