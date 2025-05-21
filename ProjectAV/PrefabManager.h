#pragma once
#include "Prefab.h"
#include "Node.h"
#include <string>

class PhysicsEngine;
class Window;

class PrefabManager {
public:
    PrefabManager(PhysicsEngine* physicsEngine, Window* wnd);
	~PrefabManager();  
    Node* InstantiateStone(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandBig1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandMedium1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandMedium2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandSmall1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;


private:
    PhysicsEngine* physicsEngine;
    Window* wnd;
};