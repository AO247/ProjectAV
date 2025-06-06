#pragma once
#include "Node.h"
#include "Component.h"
#include "Components.h"
#include "Window.h"
#include <DirectXMath.h>
#include "SoundEffectsPlayer.h"
#include "CMath.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;
class PrefabManager;
class LevelGenerator;


class SpawnJump : public Component
{
public:
	SpawnJump(Node* owner, Window& window, Node* player);
	virtual ~SpawnJump() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	Node* playerNode;
	bool activated = false;
	float jumpCooldown = 0.0f;
	Vector3 targetPosition = Vector3::Zero;
	void Activate(Vector3 targetPos);
	bool moved = true;
	bool pushed = true;
	bool playerReady = false;
	bool halfWay = false;
	bool rotated = false;
private:
	Window& wnd;
};