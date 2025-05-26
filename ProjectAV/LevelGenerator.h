#pragma once
#include "Node.h"
#include "PrefabManager.h"
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;

class LevelGenerator
{
public:
	LevelGenerator(PrefabManager* prefabManager, Node* root);
	~LevelGenerator() = default;

	int bigIslandCount = 2;
	int mediumIslandCount = 2;
	int smallIslandCount = 2;


private:
	float distance = 1.0f;
	PrefabManager* prefabManager;
	Node* pSceneRoot;
	Vector3 islandSize = { 10.0f, 1.0f, 10.0f };
	std::vector<std::string> islands;
	std::vector<Vector4> points;
	void GenerateIslands();
	void ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos);
};