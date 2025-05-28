#pragma once
#include "Prefab.h"
#include "Node.h"
#include <string>
#include "Components.h"
#include "PhysicsCommon.h"
//class PhysicsEngine;
class Window;
class ShootAttack;
class PrefabManager {
public:
    PrefabManager(Window* wnd) : wnd(wnd)
    {

    }
	~PrefabManager(); 
    static Window* wind;
    static Node* root;

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
	Node* InstantiateShootingEnemy(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1, Node* pPlayer = nullptr) const;

    static Node* InstantiateBullet(float locX, float locY, float locZ, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Bullet", nullptr, "BULLET");
        Node* pNewNode = pNewNodeOwner.get();
        root->AddChild(std::move(pNewNodeOwner));


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        BodyCreationSettings BodySettings(new JPH::BoxShape(Vec3(2.0f, 10.0f, 2.0f)), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Kinematic, Layers::WALL);
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        BodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
        BodySettings.mMassPropertiesOverride.mMass = 1.0f;
        BodySettings.mFriction = 0.0f;
        BodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, BodySettings)
        );

        pNewNode->AddComponent(
            std::make_unique<Bullet>(pNewNode)
        );

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }

private:
    Window* wnd;
};