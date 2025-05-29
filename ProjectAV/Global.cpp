#include "Global.h"

namespace dx = DirectX;
Global::Global(Node* owner, Window& window, PrefabManager* prefabMan, Node* player)
	: Component(owner), wnd(window), prefabManager(prefabMan), playerNode(player)  // Initialize reference member
{
	AddSpecialLevel();
}


void Global::Update(float dt) 
{

}

void Global::AddLevel()
{
	std::string levelName = "Level " + std::to_string(levelCount + 1);
	auto pLevel = std::make_unique<Node>(levelName);
	Node* level = pLevel.get();
	pOwner->AddChild(std::move(pLevel));
	levels.push_back(pLevel.get());
	auto prefabs = std::make_unique<Node>("Prefabs");
	Node* prefab = prefabs.get();
	level->AddChild(std::move(prefabs));
	prefabManager->root = prefab;
	LevelGenerator levelGenerator(prefabManager, level, playerNode, false);
	level->SetLocalPosition(dx::XMFLOAT3(0.0f, levelCount * 400.0f, 0.0f)); // Example position
	levelCount++;
}

void Global::AddSpecialLevel()
{
	if (currentLevelGenerator != nullptr) return;
	std::string levelName = "Level " + std::to_string(levelCount + 1);
	auto pLevel = std::make_unique<Node>(levelName);
	Node* level = pLevel.get();
	pOwner->AddChild(std::move(pLevel));
	levels.push_back(pLevel.get());
	auto prefabs = std::make_unique<Node>("Prefabs");
	Node* prefab = prefabs.get();
	level->AddChild(std::move(prefabs));
	prefabManager->root = prefab;
	level->SetLocalPosition(dx::XMFLOAT3(0.0f, levelCount * 400.0f, 0.0f)); // Example position
	if (levelCount % 2 == 0)
	{
		level->SetLocalRotation(dx::XMFLOAT3(0.0f, to_rad(180.0f), 0.0f));
	}
	level->AddComponent(
		std::make_unique<LevelGeneratorComp>(level, prefabManager, playerNode)
	);
	levelCount++;
}

void Global::DrawImGuiControls()
{

}