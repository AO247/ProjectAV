#pragma once
#include "Node.h"
#include "PrefabManager.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;

class LevelGenerator
{
public:
	LevelGenerator(PrefabManager* prefabManager, Node* root, Node* pPlayer);
	~LevelGenerator() = default;

	int bigIslandCount = 3;
	int mediumIslandCount = 0;
	int smallIslandCount = 0;

	int numberOfHardEnemies = 5;
	int numberOfMediumEnemies = 0;
	int numberOfEasyEnemies = 0;

private:
	float distance = 1.0f;
	PrefabManager* prefabManager;
	Node* pSceneRoot;
	Node* pPlayer;
	std::vector<Node*> islands;
	std::vector<Vector4> points;
	std::vector<Vector3> spawnPoints;
	void GenerateIslands();
	void SpawnEnemies();
	void ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos);
	bool Collide(Node* island1, Node* island2);
};