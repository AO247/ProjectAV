#pragma once
#include "Node.h"
#include "Component.h"
#include "Components.h"
#include "Window.h"
#include <DirectXMath.h>
#include "SoundEffectsPlayer.h"
#include "CMath.h"

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

	void Activate();
	bool playerReady = false;
	bool activated = false;
	bool upgraded = false;
	float jumpCooldown = 0.0f;
private:
	Window& wnd;
};