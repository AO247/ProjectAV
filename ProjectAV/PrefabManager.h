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
    Node* InstantiateWall(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
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
    Node* InstantiateIslandBig2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandMedium1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandMedium2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandSmall1(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;
    Node* InstantiateIslandSmall2(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1) const;

    Node* InstantiateEnemy(Node* parentNode = nullptr, float locX = 0, float locY = 0, float locZ = 0, float scale = 1, Node* pPlayer = nullptr) const;

    static Node* InstantiateShootingEnemy(Node* parentNode, float locX, float locY, float locZ, float scale, Node* pPlayer)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Enemy", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\basic2.obj")
        );
        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(2.1f, 1.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
        eBodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
        eBodySettings.mMassPropertiesOverride.mMass = 10.0f;
        eBodySettings.mFriction = 0.2f;
        eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, eBodySettings)
        );

        BodyCreationSettings a1BodySettings(new JPH::CapsuleShape(5.0f, 3.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, a1BodySettings, false)
        );
        pNewNode->AddComponent(
            std::make_unique<Walking>(pNewNode)
        );
        pNewNode->AddComponent(
            std::make_unique<ShootAttack>(pNewNode)
        );
        pNewNode->AddComponent(
            std::make_unique<BasicAttack>(pNewNode)
        );
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        pNewNode->GetComponent<StateMachine>()->attackRange = 35.0f;
        pNewNode->GetComponent<StateMachine>()->followDistance = 60.0f;
        pNewNode->GetComponent<StateMachine>()->pPlayer = pPlayer;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }

    static Node* InstantiateColumn(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        BodyCreationSettings BodySettings(new JPH::BoxShape(Vec3(2.0f, 10.0f, 2.0f)), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::WALL);
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
        BodySettings.mMassPropertiesOverride.mMass = 50.0f;
        BodySettings.mFriction = 0.0f;
        BodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), BodySettings)
        );

        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStone1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\kamien_1.2.obj")
        );

        BodyCreationSettings BodySettings(new JPH::BoxShape(Vec3(1.0f, 1.0f, 1.0f)), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mFriction = 0.5f;
        BodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), BodySettings)
        );
        Rigidbody* pRigidbody = pNewNodeOwner->GetComponent<Rigidbody>();
        pNewNodeOwner->AddComponent(
            std::make_unique<Throwable>(pNewNodeOwner.get())
        );

        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateIslandBig1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_2\\wyspa_duza_okragla_2.obj")
        );
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->AddComponent(
            std::make_unique<Island>(pNewNodeOwner.get())
        );
        Island* pIsland = pNewNodeOwner->GetComponent<Island>();
        auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
        auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
        auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
        auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-61.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(69.0f, 0.0f, -10.0f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 64.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -64.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
            std::make_unique<ModelComponent>(upPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        downPoint->AddComponent(
            std::make_unique<ModelComponent>(downPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        leftPoint->AddComponent(
            std::make_unique<ModelComponent>(leftPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        rightPoint->AddComponent(
            std::make_unique<ModelComponent>(rightPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        /*InstantiateStone1(pNewNode, 22.2f, 30.0f, 32.2f, 2.0f);
        InstantiateStone1(pNewNode, 4.2f, 30.0f, 17.2f, 2.0f);
        InstantiateStone1(pNewNode, -10.2f, 30.0f, -2.2f, 2.0f);*/
        InstantiateStone1(pNewNode, 22.2f, 30.0f, -18.2f, 2.0f);
        InstantiateStone1(pNewNode, -17.2f, 30.0f, 22.2f, 2.0f);
        InstantiateStone1(pNewNode, 11.2f, 30.0f, -8.2f, 2.0f);
      /*  InstantiateColumn(pNewNode, 20.0f, 0.0f, 25.0f, 1.0f);
        InstantiateColumn(pNewNode, -5.0f, 0.0f, 25.0f, 1.0f);
        InstantiateColumn(pNewNode, 20.0f, 0.0f, -12.0f, 1.0f);
        InstantiateColumn(pNewNode, -5.0f, 0.0f, -12.0f, 1.0f);*/
               ////Node* platform1 = InstantiatePlatform1(pNewNode, 41.0f, 0.0f, 6.0f, 1.0f);
               ////Node* platform3 = InstantiatePlatform3(pNewNode, -19.0f, 0.0f, -26.0f, 1.0f);


        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(23.0f, 8.0f, 33.0f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(5.0f, 8.0f, 18.0f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(-11.0f, 8.0f, -3.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));



        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;

    }

    static Node* InstantiateFirstIsland(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Spawn", nullptr, "SPAWN");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_2\\wyspa_mala_2.obj")
        );
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->AddComponent(
            std::make_unique<Island>(pNewNodeOwner.get())
        );
        Island* pIsland = pNewNodeOwner->GetComponent<Island>();
        auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
        auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
        auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
        auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 36.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -24.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-35.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(35.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
            std::make_unique<ModelComponent>(upPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        downPoint->AddComponent(
            std::make_unique<ModelComponent>(downPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        leftPoint->AddComponent(
            std::make_unique<ModelComponent>(leftPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        rightPoint->AddComponent(
            std::make_unique<ModelComponent>(rightPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateColumn(pNewNode, 20.0f, 0.0f, 20.0f, 4.0f);
        InstantiateColumn(pNewNode, 20.0f, 0.0f, -20.0f, 4.0f);
        InstantiateColumn(pNewNode, -20.0f, 0.0f, 20.0f, 4.0f);
        InstantiateColumn(pNewNode, -20.0f, 0.0f, -20.0f, 4.0f);



        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }



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
    Window* wnd;
private:
};