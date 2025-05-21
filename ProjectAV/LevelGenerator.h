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

	float islandCount = 20.0f;

private:
	float distance = 1.0f;
	PrefabManager* prefabManager;
	Node* pSceneRoot;
	Vector3 islandSize = { 10.0f, 1.0f, 10.0f };
	std::vector<std::string> islands;
	std::vector<Vector4> points;
	void GenerateIslands();
};