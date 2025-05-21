#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;

class Island : public Component
{
public:
	Island(Node* owner);
	virtual ~Island() = default;

	void Rotate();

	Node* leftPoint;
	Node* rightPoint;
	Node* upPoint;
	Node* downPoint;
private:

};