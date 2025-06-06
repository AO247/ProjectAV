#include "Global.h"
namespace dx = DirectX;
Global::Global(Node* owner, Window& window, Node* player)
	: Component(owner), wnd(window), playerNode(player)  // Initialize reference member
{
	PrefabManager::InstantiateStartIsland(pOwner, 0.0f, 0.0f, 0.0f, 4.0f);
	player->SetLocalPosition(enterPoint);
	firstSpawn = PrefabManager::InstantiateFirstIsland(pOwner, 0.0f, 5.0f, -20.0f, 1.0f);
	StartRun();
}


void Global::Update(float dt) 
{
	if (started)
	{
		if (levels[levels.size() - 3]->GetComponent<LevelGenerator>()->isFinished) {
			if (levels[levels.size() - 3]->FindAllChildrenByTag("ENEMY").size() <= 0)
			{
				if (!completed)
				{
					spawn = levels[levels.size() - 3]->FindFirstChildByTag("SPAWN");
					Node* target = levels[levels.size() - 2]->FindFirstChildByTag("SPAWN");
					spawn->GetComponent<SpawnJump>()->Activate(target->GetWorldPosition());
					completed = true;
				}
				else if (spawn->GetComponent<SpawnJump>()->halfWay)
				{
					levels[levels.size() - 2]->GetComponent<LevelGenerator>()->startEnemyGenerating = true;
					levels[levels.size() - 5]->Destroy();
					AddSpecialLevel();
					currentLevel++;
				}
			}
		}

	}
	else 
	{
		if (firstSpawn->GetComponent<SpawnJump>()->pushed)
		{
			levels[levels.size() - 3]->GetComponent<LevelGenerator>()->startEnemyGenerating = true;
			currentLevel = 1;
			started = true;
		}
	}
	if (playerNode->GetLocalPosition().y < (currentLevel * 400.0f) - 350.0f || playerNode->GetComponent<Health>()->currentHealth == 0.0f)
	{
		EndRun();
	}
	if (ending)
	{
		levels[endingRemover]->Destroy();
		endingRemover--;
		if(endingRemover < 0)
		{
			StartRun();
			ending = false;
		}
	}
	if (firstRun == 0)
	{
		firstSpawn->GetComponent<SpawnJump>()->Activate(levels[levels.size() - 3]->FindFirstChildByTag("SPAWN")->GetWorldPosition());
		firstRun--;
	}
	else if (firstRun > 0)
	{
		firstRun--;
	}
}


void Global::AddSpecialLevel()
{
	std::string levelName = "Level " + std::to_string(levelCount);
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
	completed = false;
	levelCount++;
}

void Global::StartRun()
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
	if(firstRun == -1)
	{
		firstRun = 10;
	}
}

void Global::EndRun()
{
	playerNode->GetComponent<PlayerController>()->abilitySlot1->GetComponent<Ability>()->objects.clear();
	playerNode->GetComponent<PlayerController>()->abilitySlot2->GetComponent<Ability>()->objects.clear();
	playerNode->GetComponent<PlayerController>()->abilitySlot3->GetComponent<Ability>()->objects.clear();
	ending = true;
	endingRemover = levels.size() - 1;
	playerNode->GetComponent<Health>()->currentHealth = playerNode->GetComponent<Health>()->maxHealth;
	playerNode->GetComponent<PlayerController>()->alive = true;
	playerNode->SetLocalPosition(enterPoint);
	currentLevel = 0;
	levelCount = 1;
	wnd.playerLocked = false;
	started = false;
}
void Global::DrawImGuiControls()
{
	ImGui::Checkbox("Started", &started);
	ImGui::Checkbox("Ending", &ending);
	ImGui::Text("Current Level: %d", currentLevel);
	ImGui::Text("Level Count: %d", levelCount);
	ImGui::Text("Levels: %d", levels.size());
	ImGui::Text("Completed: %d", completed);
	ImGui::Text("First Run: %d", firstRun);

}