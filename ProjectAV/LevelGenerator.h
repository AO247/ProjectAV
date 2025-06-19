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

	int bigIslandCount = 2;
	int mediumIslandCount = 1;
	int smallIslandCount = 2;

	int numberOfHardEnemies = 1;
	int numberOfMediumEnemies = 1;
	int numberOfEasyEnemies = 1;


	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	bool islandGenerated = false;
	bool enemiesSpawned = false;
	bool isFinished = false;
	bool startEnemyGenerating = false;
private:
	Node* pPlayer;
	struct islandInfoStruct {
		std::string name = "";
		Vector3 pos = { 0.0f, 0.0f, 0.0f };
		float rot = 0.0f;
		Vector3 leftPoint{ 0.0f, 0.0f, 0.0f };
		Vector3 upPoint{ 0.0f, 0.0f, 0.0f };
		Vector3 rightPoint{ 0.0f, 0.0f, 0.0f };
		Vector3 downPoint{ 0.0f, 0.0f, 0.0f };
	};
	float distance = 1.0f;
	std::vector<Node*> islands;
	std::vector<islandInfoStruct> islandsInfo;
	islandInfoStruct islandInfo;
	std::vector<std::unique_ptr<Node>> islandsOwners;
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

	int counterToRotate = 0;
	float enemiesLimitTimer = 10.0f;
	float islandLimitTimer = 40.0f;
	void Rotate(int rot = 0);

	void ChangePosition(islandInfoStruct* island, Vector3 pointPos, Vector3 startPos);
	bool Collide(islandInfoStruct island1, islandInfoStruct island2);


};