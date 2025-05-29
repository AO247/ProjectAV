#pragma once
#include "Node.h"
#include "Component.h"
#include "PrefabManager.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;
class PrefabManager;

class LevelGeneratorComp : public Component
{
public:
	LevelGeneratorComp(Node* owner, Node* pPlayer);
	virtual ~LevelGeneratorComp() = default;

	int bigIslandCount = 3;
	int mediumIslandCount = 1;
	int smallIslandCount = 0;

	int numberOfHardEnemies = 1;
	int numberOfMediumEnemies = 0;
	int numberOfEasyEnemies = 0;


	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	bool islandGenerated = false;
	bool enemiesSpawned = false;
	bool isFinished = false;
private:
	Node* pPlayer;
	float distance = 1.0f;
	std::vector<Node*> islands;
	std::vector<Vector4> points;
	std::vector<Vector3> spawnPoints;

	void GenerateIslands();
	void SpawnEnemies();

	bool spawned = true;
	Node* islandPrefab = nullptr;
	Node* enemy = nullptr;
	int islandNumber = 1;
	int pIslandNumber = 1;
	Island* island = nullptr;

	int counterToRotate = 0;

	void ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos);
	bool Collide(Node* island1, Node* island2);
};