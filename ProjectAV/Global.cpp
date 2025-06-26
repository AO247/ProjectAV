#include "Global.h"
namespace dx = DirectX;
Global::Global(Node* owner, Window& window, Node* player, Node* base)
	: Component(owner), wnd(window), playerNode(player), base(base)
{
	auto pLevels = std::make_unique<Node>("Levels");
	levelsNode = pLevels.get();
	pOwner->AddChild(std::move(pLevels));

}


void Global::Update(float dt) 
{
	if (!startedGame) return;
	if (started)
	{
		if (levels[2]->GetComponent<LevelGenerator>()->isFinished) {
			if (levels[2]->FindAllChildrenByTag("ENEMY").size() <= 0)
			{
				if (!completed)
				{
					spawn = levels[2]->FindFirstChildByTag("SPAWN");
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
			levels[2]->GetComponent<LevelGenerator>()->startEnemyGenerating = true;
			levels[2]->GetComponent<StoneGenerator>()->active = true;
			currentLevel = 1;
			started = true;
			drawLoadingScreen = false;
		}
		if (levels[4])
		{
			if (!(levels[4]->GetComponent<LevelGenerator>()->islandGenerated) && !(levels[3]->GetComponent<LevelGenerator>()->islandGenerated)
				&& !(levels[2]->GetComponent<LevelGenerator>()->islandGenerated))
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
		StartRun();
	}
	/*if (ending)
	{
		if (levels[endingRemover] != nullptr)
		{
			levels[endingRemover]->Destroy();
		}
		endingRemover--;
		if(endingRemover < 0)
		{
			levels.clear();
			StartRun();
			ending = false;
		}
	}*/
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
	levels[0]->Destroy();
	auto it = levels.begin();
	levels.erase(it);
	AddSpecialLevel();
	levels[2]->GetComponent<LevelGenerator>()->startEnemyGenerating = true;
	levels[2]->GetComponent<StoneGenerator>()->active = true;
	levels[1]->GetComponent<StoneGenerator>()->active = false;

	currentLevel++;
}

void Global::AddSpecialLevel()
{
	std::string levelName = "Level " + std::to_string(levelCount);
	auto pLevel = std::make_unique<Node>(levelName);
	Node* level = pLevel.get();
	levelsNode->AddChild(std::move(pLevel));
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
	level->AddComponent(
		std::make_unique<StoneGenerator>(level)
	);
	if (levelCount == 1)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 0;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 0;

		gen->numberOfHardEnemies = 0;
		gen->numberOfMediumEnemies = 0;
		gen->numberOfEasyEnemies = 2;
	}
	else if (levelCount == 2)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 0;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 1;

		gen->numberOfHardEnemies = 0;
		gen->numberOfMediumEnemies = 1;
		gen->numberOfEasyEnemies = 1;
	}
	else if (levelCount == 3)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 1;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 1;

		gen->numberOfHardEnemies = 1;
		gen->numberOfMediumEnemies = 1;
		gen->numberOfEasyEnemies = 1;
	}
	else if (levelCount == 4)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 0;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 3;

		gen->numberOfHardEnemies = 2;
		gen->numberOfMediumEnemies = 0;
		gen->numberOfEasyEnemies = 1;
	}
	else if (levelCount == 7)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 1;
		gen->mediumIslandCount = 0;
		gen->smallIslandCount = 1;

		gen->numberOfHardEnemies = 0;
		gen->numberOfMediumEnemies = 3;
		gen->numberOfEasyEnemies = 0;
		/*gen->enemiesSpawned = true;
		gen->islandGenerated = true;
		gen->enemiesSpawned = true;
		PrefabManager::InstantiateFirstIsland(level, Vector3(0.0f, 0.0f, 20.0f), 1.0f);
		PrefabManager::InstantiateBossIsland(level, Vector3(0.0f, 5.0f, 160.0f), 1.0f);
		PrefabManager::InstantiateBossEnemy(level, Vector3(0.0f, 15.0f, 80.0f));*/

	}
	else if (levelCount == 6)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 1;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 1;

		gen->numberOfHardEnemies = 2;
		gen->numberOfMediumEnemies = 0;
		gen->numberOfEasyEnemies = 1;
	}
	else if (levelCount == 5)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 0;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 0;

		gen->numberOfHardEnemies = 0;
		gen->numberOfMediumEnemies = 0;
		gen->numberOfEasyEnemies = 2;
	}
	else if (levelCount == 8)
	{
		LevelGenerator* gen = level->GetComponent<LevelGenerator>();
		gen->bigIslandCount = 1;
		gen->mediumIslandCount = 1;
		gen->smallIslandCount = 3;

		gen->numberOfHardEnemies = 3;
		gen->numberOfMediumEnemies = 2;
		gen->numberOfEasyEnemies = 2;
	}
	
	completed = false;
	levelCount++;
}


void Global::Start()
{
	startIsland = PrefabManager::InstantiateStartIsland(base, Vector3(0.0f, 0.0f, 0.0f), 1.0f);
	firstSpawn = PrefabManager::InstantiateFirstIsland(base, Vector3(0.0f, 5.0f, -20.0f), 1.0f);
	firstSpawn->GetComponent<SpawnJump>()->Activate();
	firstSpawn->GetComponent<SpawnJump>()->upgraded = true;
	startedGame = true;
	StartRun();
}
void Global::Reset()
{
	EndRun();
	startedGame = false;
	firstSpawn->Destroy();
	startIsland->Destroy();
}

void Global::StartRun()
{
	auto pLevel = std::make_unique<Node>("Level 0-1");
	Node* level = pLevel.get();
	levelsNode->AddChild(std::move(pLevel));
	levels.push_back(level);
	auto pLevel2 = std::make_unique<Node>("Level 0-2");
	Node* level2 = pLevel2.get();
	levelsNode->AddChild(std::move(pLevel2));
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
	playerNode->GetComponent<Health>()->currentHealth = playerNode->GetComponent<Health>()->maxHealth;
	playerNode->GetComponent<PlayerController>()->alive = true;
	upgradeHandler->ResetUpgrades();
	/*if (!tut->completed)
	{
		playerNode->SetLocalPosition(tut->checkpoints[tut->currentCheckpointIndex]);
		return;
	}*/
	levels[0]->Destroy();
	levels[1]->Destroy();
	levels[2]->Destroy();
	levels[3]->Destroy();
	levels[4]->Destroy();
	levels.clear();
	ending = true;
	endingRemover = levels.size() - 1;
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