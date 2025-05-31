#include "Global.h"

namespace dx = DirectX;
Global::Global(Node* owner, Window& window, Node* player)
	: Component(owner), wnd(window), playerNode(player)  // Initialize reference member
{
	auto pLevel = std::make_unique<Node>("Level 0-1");
	Node* level = pLevel.get();
	pOwner->AddChild(std::move(pLevel));
	levels.push_back(level);
	auto pLevel2 = std::make_unique<Node>("Level 0-2");
	Node* level2 = pLevel2.get();
	pOwner->AddChild(std::move(pLevel2));
	levels.push_back(level2);
	AddSpecialLevel();
	AddSpecialLevel();
	AddSpecialLevel();
}


void Global::Update(float dt) 
{
	if (levels[levels.size() - 3]->GetComponent<LevelGenerator>()->isFinished) {
		if (levels[levels.size() - 3]->FindAllChildrenByTag("ENEMY").size() <= 0)
		{
			levels[levels.size() - 5]->Destroy();
			AddSpecialLevel();
			completed = true;
		}
	}
	if (completed)
	{
		Node* spawn = levels[levels.size() - 4]->FindFirstChildByTag("SPAWN");
		if (spawn != nullptr)
		{
			if((spawn->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 8.0f)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 280.0f, 0.0f));
				completed = false;
			}
		}
	}
}


void Global::AddSpecialLevel()
{
	if (currentLevelGenerator != nullptr) return;
	std::string levelName = "Level " + std::to_string(levelCount + 1);
	auto pLevel = std::make_unique<Node>(levelName);
	Node* level = pLevel.get();
	pOwner->AddChild(std::move(pLevel));
	levels.push_back(level);
	auto prefabs = std::make_unique<Node>("Prefabs");
	Node* prefab = prefabs.get();
	level->AddChild(std::move(prefabs));
	level->SetLocalPosition(dx::XMFLOAT3(0.0f, levelCount * 400.0f, 0.0f)); // Example position
	if (levelCount % 2 == 0)
	{
		level->AddComponent(
			std::make_unique<LevelGenerator>(level, playerNode, true)
		);
	}
	else 
	{
		level->AddComponent(
			std::make_unique<LevelGenerator>(level, playerNode, false)
		);
	}

	levelCount++;
}

void Global::DrawImGuiControls()
{

}