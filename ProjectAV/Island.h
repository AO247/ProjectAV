#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Rigidbody.h"
#include "SoundEffectsPlayer.h"

class Node;

class Island : public Component
{
public:
	Island(Node* owner);
	virtual ~Island() = default;

	void Rotate(int rot = 0.0f);

	Node* leftPoint;
	Node* rightPoint;
	Node* upPoint;
	Node* downPoint;
	Vector3 halfExtents;

	std::vector<Node*> spawnPoints;
private:

};