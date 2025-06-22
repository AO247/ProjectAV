#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"

class Node;

class Spikes : public Component
{
public:
	Spikes(Node* owner, std::string tag = "ATTACK");
	virtual ~Spikes() = default;
	void OnCollisionEnter(Node* other) override;
	virtual void DrawImGuiControls() override;
private:
};