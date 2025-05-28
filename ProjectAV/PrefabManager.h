#pragma once
#include "Prefab.h"
#include "Node.h"
#include <string>

//class PhysicsEngine;
class Window;

class PrefabManager {
public:
    PrefabManager(Window* wnd) : wnd(wnd)
    {

    }
	~PrefabManager(); 
    Node* InstantiateMushroom1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateMushroom2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateMushroom3(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateColumn(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateWall(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateStone1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateStone2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateStone3(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateStone4(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateStone5(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiatePlatform1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiatePlatform2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiatePlatform3(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiatePlatform4(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiatePlatform5(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiatePlatform6(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandBig1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandBig2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandMedium1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandMedium2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandSmall1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandSmall2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;

    Node* InstantiateEnemy(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1, Node* pPlayer = nullptr) const;

private:
    Window* wnd;
};