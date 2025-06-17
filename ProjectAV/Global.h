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

class Node;
class PrefabManager;
class LevelGenerator;


class Global : public Component
{
public:
	Global(Node* owner, Window& window, Node* player, Node* base);
	virtual ~Global() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	Node* playerNode;
	Node* spawn;
	Node* firstSpawn;
	Node* levelsNode;
	Node* base;
	Tutorial* tut;
	std::vector<Node*> enemies;
	std::vector<Node*> levels;
	int levelCount = 1;
	int currentLevel = 0;
	void AddSpecialLevel();
	bool completed = false;
	bool started = false;
	bool ending = false;
	int endingRemover = 0;
	Vector3 enterPoint = { 0.0f, 5.0f, -60.0f };
	bool upgradeOpen = false;
	bool drawLoadingScreen = true;
	UpgradeHandler* upgradeHandler;
	void NextStage();

private:
	Window& wnd;
	void AddLevel();
	void StartRun();
	void EndRun();
};