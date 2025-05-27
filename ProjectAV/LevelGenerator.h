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

	int bigIslandCount = 1;
	int mediumIslandCount = 0;
	int smallIslandCount = 0;


private:
	float distance = 1.0f;
	PrefabManager* prefabManager;
	Node* pSceneRoot;
	std::vector<Node*> islands;
	std::vector<Vector4> points;
	void GenerateIslands();
	void ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos);
	bool Collide(Node* island1, Node* island2);
};