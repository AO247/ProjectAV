#pragma once
#include <string>
#include "Collider.h"
#include "Node.h"

class PhysicsEngine;
class Window;

class Prefab
{
public:
	Prefab(const std::string& modelPath, PhysicsEngine* engine, Window* window);
	Prefab();
	~Prefab();
	const std::string& GetModelPath() const;
	void SetModelPath(const std::string& path);
	Node* InstantiatePrefab(const std::string& prefabName, Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;

private:
	std::string modelPath;
	PhysicsEngine* physicsEngine;
	Window* wnd;

};

