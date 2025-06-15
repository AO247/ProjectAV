#include "Global.h"
namespace dx = DirectX;
Global::Global(Node* owner, Window& window, Node* player)
	: Component(owner), wnd(window), playerNode(player)
{

	player->SetLocalPosition(enterPoint);
	PrefabManager::InstantiateStartIsland(pOwner, Vector3(0.0f, 0.0f, 0.0f), 4.0f);
	firstSpawn = PrefabManager::InstantiateFirstIsland(pOwner, Vector3(0.0f, 5.0f, -20.0f), 1.0f);
	firstSpawn->GetComponent<SpawnJump>()->Activate();
	firstSpawn->GetComponent<SpawnJump>()->upgraded = true;
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
					spawn->GetComponent<SpawnJump>()->Activate();
					completed = true;

				}
			}
		}

	}
	else 
	{
		if (playerNode->GetLocalPosition().y > 30.0f)
		{
			levels[levels.size() - 3]->GetComponent<LevelGenerator>()->startEnemyGenerating = true;
			currentLevel = 1;
			started = true;
			drawLoadingScreen = false;
		}
		if (levels[levels.size() - 1] && !ending)
		{
			if (!(levels[levels.size() - 1]->GetComponent<LevelGenerator>()->islandGenerated) && !(levels[levels.size() - 2]->GetComponent<LevelGenerator>()->islandGenerated)
				&& !(levels[levels.size() - 3]->GetComponent<LevelGenerator>()->islandGenerated))
			{
				drawLoadingScreen = true;
			}
			else
			{
				drawLoadingScreen = false;
			}
		}
	}
	if (playerNode->GetLocalPosition().y < ((currentLevel - 1) * 400.0f) - 50.0f || playerNode->GetComponent<Health>()->currentHealth <= 0.0f)
	{
		EndRun();
	}
	if (ending)
	{
		if (levels[endingRemover] != nullptr)
		{
			levels[endingRemover]->Destroy();
		}
		endingRemover--;
		if(endingRemover < 0)
		{
			StartRun();
			ending = false;
		}
	}
	if (!upgradeHandler->upgradeMenuOpen && upgradeOpen)
	{
		upgradeOpen = false;
		levels[levels.size() - 4]->FindFirstChildByTag("SPAWN")->GetComponent<SpawnJump>()->upgraded = true;
	}
}
void Global::NextStage() 
{
	upgradeHandler->ShowUpgradeMenu();
	upgradeOpen = true;
	levels[levels.size() - 2]->GetComponent<LevelGenerator>()->startEnemyGenerating = true;
	levels[levels.size() - 5]->Destroy();
	AddSpecialLevel();
	currentLevel++;
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
	level->SetLocalPosition(dx::XMFLOAT3(0.0f, levelCount * 400.0f, 0.0f));
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
}