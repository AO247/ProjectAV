#pragma once
#include "Node.h"
#include "Component.h"
#include "Components.h"
#include "Window.h"
#include <DirectXMath.h>
#include "SoundEffectsPlayer.h"
#include "PrefabManager.h"
#include "LevelGenerator.h"
#include "CMath.h"
#include "LevelGeneratorComp.h"

// Forward declare Node to avoid circular include if necessary,
// but including Node.h is often fine here.
class Node;
class PrefabManager;
class LevelGenerator;


class Global : public Component
{
public:
	Global(Node* owner, Window& window, Node* player);
	virtual ~Global() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	Node* playerNode;
	std::vector<Node*> enemies;
	std::vector<Node*> levels;
	int levelCount = 0;
	void AddSpecialLevel();
	bool yey = true;
	bool completed = false;
private:
	Window& wnd;
	std::unique_ptr<LevelGenerator> currentLevelGenerator;
	void AddLevel();
};