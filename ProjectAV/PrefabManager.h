#pragma once
#include "Prefab.h"
#include "Node.h"
#include <string>
#include "btBulletDynamicsCommon.h"

//class PhysicsEngine;
class Window;

class PrefabManager {
public:
    PrefabManager(btDiscreteDynamicsWorld* physicsWorld, Window* wnd) : physicsWorld(physicsWorld), wnd(wnd)
    {

    }
	~PrefabManager();  
    Node* InstantiateStone(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIsland(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;


private:
    btDiscreteDynamicsWorld* physicsWorld;
    Window* wnd;
};