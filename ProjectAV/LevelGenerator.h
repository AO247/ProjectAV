#pragma once
#include "Node.h"
#include "PrefabManager.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;
class PrefabManager;

class LevelGenerator
{
public:
	LevelGenerator(PrefabManager* prefabManager, Node* root, Node* pPlayer, bool isNew);
	~LevelGenerator() = default;

	int bigIslandCount = 0;
	int mediumIslandCount = 0;
	int smallIslandCount = 0;

	int numberOfHardEnemies = 0;
	int numberOfMediumEnemies = 0;
	int numberOfEasyEnemies = 0;


	void Update();
	bool islandGenerated = false;
	bool enemiesSpawned = false;
	bool isFinished = false;
private:
	PrefabManager* prefabManager;
	Node* pSceneRoot;
	Node* pPlayer;
	float distance = 1.0f;
	std::vector<Node*> islands;
	std::vector<Vector4> points;
	std::vector<Vector3> spawnPoints;
	void GenerateIslands();
	void SpawnEnemies();

	void GenerateIslandsNew();
	void SpawnEnemiesNew();

	bool spawned = true;


	void ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos);
	bool Collide(Node* island1, Node* island2);
};