#pragma once
#include "Node.h"
#include "Component.h"
#include "PrefabManager.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;
class PrefabManager;

class LevelGenerator : public Component
{
public:
	LevelGenerator(Node* owner, Node* pPlayer, bool rot);
	virtual ~LevelGenerator() = default;

	int bigIslandCount = 1;
	int mediumIslandCount = 0;
	int smallIslandCount = 3;

	int numberOfHardEnemies = 3;
	int numberOfMediumEnemies = 3;
	int numberOfEasyEnemies = 3;


	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	bool islandGenerated = false;
	bool enemiesSpawned = false;
	bool isFinished = false;
	bool startEnemyGenerating = false;
private:
	Node* pPlayer;
	float distance = 1.0f;
	std::vector<Node*> islands;
	std::vector<Vector4> points;
	std::vector<Vector3> spawnPoints;

	void GenerateIslands();
	void SpawnEnemies();
	bool spawnNeedRotation = false;
	bool spawned = true;
	Node* islandPrefab = nullptr;
	Node* enemy = nullptr;
	int islandNumber = 0;
	int pIslandNumber = 1;
	Island* island = nullptr;

	int counterToRotate = 0;

	void ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos);
	bool Collide(Node* island1, Node* island2);
};