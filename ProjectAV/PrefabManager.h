#pragma once
#include "Node.h"
#include <string>
#include "Components.h"
#include "PhysicsCommon.h"
#include "MainRenderGraph.h"
#include "Window.h"

//class PhysicsEngine;
class ShootAttack;
class PrefabManager {
public:
	PrefabManager(Window* wnd) : wnd(wnd)
    {

    }
	~PrefabManager(); 
    static Window* wind;
    static Node* root;
    static Node* player;
	static Rgph::MainRenderGraph* rg;


    static Node* InstantiateMushroom1(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\grzyb_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateMushroom2(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\grzyb_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateMushroom3(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\grzyb_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateSpike1(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kolce_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateSpike2(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kolce_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateFaceColumn1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\filar_twarze.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateFire1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\krata_ogien.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateWall(Node* parentNode, float locX, float locY, float locZ, float scale) {

    }
    static Node* InstantiateRock1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\filar_twarze.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateRock2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\filar_twarze.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateRock3(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\filar_twarze.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateRock4(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\filar_twarze.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateStick1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\patyk_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateStick2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\patyk_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateStone2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateStone3(Node* parentNode, float locX, float locY, float locZ, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateStone4(Node* parentNode, float locX, float locY, float locZ, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_4.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateStone5(Node* parentNode, float locX, float locY, float locZ, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_5.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateLog1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\konar_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiateLog2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\konar_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiatePot1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiatePot2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiatePot3(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    static Node* InstantiatePot4(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_4.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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
    Node* InstantiatePlatform4(Node* parentNode, float locX, float locY, float locZ, float scale) const;
    Node* InstantiatePlatform5(Node* parentNode, float locX, float locY, float locZ, float scale) const;
    Node* InstantiatePlatform6(Node* parentNode, float locX, float locY, float locZ, float scale) const;
    static Node* InstantiateIslandBig2(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_0606\\wyspa_duza_p.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

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
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -27.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        //upPoint->AddComponent(
        //    std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //downPoint->AddComponent(
        //    std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //leftPoint->AddComponent(
        //    std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //rightPoint->AddComponent(
        //    std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, 3.0f, 30.0f, -8.0f, 2.0f);
        InstantiateStone1(pNewNode, 20.2f, 30.0f, 4.2f, 2.0f);
        InstantiateStone1(pNewNode, 15.2f, 30.0f, 21.2f, 2.0f);
        InstantiateStone1(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
        InstantiateStone1(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
        InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
        InstantiateColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
        InstantiateColumn(pNewNode, 16.5f, 0.0f, 10.0f, 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-11.0f, 2.0f, 3.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandMedium1(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_0606\\wyspa_srednia.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

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
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -27.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        //upPoint->AddComponent(
        //    std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //downPoint->AddComponent(
        //    std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //leftPoint->AddComponent(
        //    std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //rightPoint->AddComponent(
        //    std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, 3.0f, 30.0f, -8.0f, 2.0f);
        InstantiateStone1(pNewNode, 20.2f, 30.0f, 4.2f, 2.0f);
        InstantiateStone1(pNewNode, 15.2f, 30.0f, 21.2f, 2.0f);
        InstantiateStone1(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
        InstantiateStone1(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
        InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
        InstantiateColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
        InstantiateColumn(pNewNode, 16.5f, 0.0f, 10.0f, 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-11.0f, 2.0f, 3.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandMedium2(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_0606\\wyspa_srednia.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

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
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -27.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        //upPoint->AddComponent(
        //    std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //downPoint->AddComponent(
        //    std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //leftPoint->AddComponent(
        //    std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //rightPoint->AddComponent(
        //    std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, 3.0f, 30.0f, -8.0f, 2.0f);
        InstantiateStone1(pNewNode, 20.2f, 30.0f, 4.2f, 2.0f);
        InstantiateStone1(pNewNode, 15.2f, 30.0f, 21.2f, 2.0f);
        InstantiateStone1(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
        InstantiateStone1(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
        InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
        InstantiateColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
        InstantiateColumn(pNewNode, 16.5f, 0.0f, 10.0f, 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-11.0f, 2.0f, 3.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandSmall1(Node* parentNode, float locX, float locY, float locZ, float scale) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_0606\\wyspa_mala.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

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
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -27.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        //upPoint->AddComponent(
        //    std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //downPoint->AddComponent(
        //    std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //leftPoint->AddComponent(
        //    std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //rightPoint->AddComponent(
        //    std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, 3.0f, 30.0f, -8.0f, 2.0f);
        InstantiateStone1(pNewNode, 20.2f, 30.0f, 4.2f, 2.0f);
        InstantiateStone1(pNewNode, 15.2f, 30.0f, 21.2f, 2.0f);
        InstantiateStone1(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
        InstantiateStone1(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
        InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
        InstantiateColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
        InstantiateColumn(pNewNode, 16.5f, 0.0f, 10.0f, 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-11.0f, 2.0f, 3.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }



    static Node* InstantiateStatue(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Test", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kolce_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
    static Node* InstantiateColumn(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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

        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateTopColumn(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Top", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\objects\\kolumna_1_top.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 10.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
        BodySettings.mFriction = 0.5f;
        BodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), BodySettings)
        );

        pNewNodeOwner->AddComponent(
            std::make_unique<BrokenPart>(pNewNodeOwner.get())
        );
        pNewNodeOwner->AddComponent(
            std::make_unique<Throwable>(pNewNodeOwner.get())
        );

        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateMiddleColumn(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Middle", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\objects\\kolumna_1_srodek.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 40.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
        BodySettings.mFriction = 0.5f;
        BodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), BodySettings)
        );
        pNewNodeOwner->AddComponent(
            std::make_unique<Throwable>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<Throwable>()->speed = 2.0f;
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateBaseColumn(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Base", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\objects\\kolumna_1_podstawa.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings bodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::WALL);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateNewColumn(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("New Column", nullptr, "WALL");
        Node* pNewNode = pNewNodeOwner.get();

        InstantiateBaseColumn(pNewNode, 0.0f, 2.0f, 0.0f, 1.0f);
        Node* middle = InstantiateMiddleColumn(pNewNode, 0.0f, 12.6f, 0.0f, 1.0f);
        Node* top = InstantiateTopColumn(middle, 0.0f, 7.9f, 0.0f, 1.0f);
        top->GetComponent<Rigidbody>()->ConnectWithOtherBody(middle->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 7.9f, 0.0f));

        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateStone1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\kamien_1.2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        ScaledShapeSettings islandScaling(islandShape, Vec3Arg(scale, scale, scale));
        islandShape = islandScaling.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 7.0f;
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
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

    static Node* InstantiatePlatform1(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_glazy_2\\platforma_1.2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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


        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiatePlatform2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_glazy_2\\platforma_2.2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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


        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiatePlatform3(Node* parentNode, float locX, float locY, float locZ, float scale)
    {


        auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_glazy_2\\platforma_3.3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-65.0f, 0.0f, 35.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(65.0f, 0.0f, 20.0f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(40.0f, 0.0f, 65.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(30.0f, 0.0f, -65.0f));
        pIsland->halfExtents = { 63.0f, 2.0f, 63.0f };
        /*pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };*/
        /*upPoint->AddComponent(
            std::make_unique<ModelComponent>(upPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        upPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        downPoint->AddComponent(
            std::make_unique<ModelComponent>(downPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        downPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        leftPoint->AddComponent(
            std::make_unique<ModelComponent>(leftPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        leftPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        rightPoint->AddComponent(
            std::make_unique<ModelComponent>(rightPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
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
        InstantiateNewColumn(pNewNode, 20.0f, 0.0f, 25.0f, 1.0f);
        InstantiateNewColumn(pNewNode, -5.0f, 0.0f, 25.0f, 1.0f);
        InstantiateNewColumn(pNewNode, 20.0f, 0.0f, -12.0f, 1.0f);
        InstantiateNewColumn(pNewNode, -5.0f, 0.0f, -12.0f, 1.0f);
        InstantiatePlatform1(pNewNode, 41.0f, 0.0f, 6.0f, 1.0f);
        InstantiatePlatform3(pNewNode, -19.0f, 0.0f, -26.0f, 1.0f);
		//InstantiateStatue(pNewNode, 0.0f, 0.0f, 0.0f, 1.0f);


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

    static Node* InstantiateIslandSmall2(Node* parentNode, float locX, float locY, float locZ, float scale)
    {


        auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_2\\wyspa_mala_L_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

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
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -27.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        //upPoint->AddComponent(
        //    std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //downPoint->AddComponent(
        //    std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //leftPoint->AddComponent(
        //    std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //rightPoint->AddComponent(
        //    std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, 3.0f, 30.0f, -8.0f, 2.0f);
        InstantiateStone1(pNewNode, 20.2f, 30.0f, 4.2f, 2.0f);
        InstantiateStone1(pNewNode, 15.2f, 30.0f, 21.2f, 2.0f);
        InstantiateStone1(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
        InstantiateStone1(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
        InstantiateNewColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
        InstantiateNewColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
        InstantiateNewColumn(pNewNode, 16.5f, 0.0f, 10.0f, 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-11.0f, 2.0f, 3.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

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
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
        pNewNodeOwner->AddComponent(
            std::make_unique<SpawnJump>(pNewNodeOwner.get(), *wind, player)
        );
        Island* pIsland = pNewNodeOwner->GetComponent<Island>();
        auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
        auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
        auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
        auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 36.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -30.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-35.0f, 0.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(35.0f, 0.0f, -4.0f));
        pIsland->halfExtents = { 33.0f, 2.0f, 33.0f };
        /* pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                 2.0f,
                                 (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };*/
                                 /*upPoint->AddComponent(
                                     std::make_unique<ModelComponent>(upPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
                                 );
                                 upPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
                                 downPoint->AddComponent(
                                     std::make_unique<ModelComponent>(downPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
                                 );
                                 downPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
                                 leftPoint->AddComponent(
                                     std::make_unique<ModelComponent>(leftPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
                                 );
                                 leftPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
                                 rightPoint->AddComponent(
                                     std::make_unique<ModelComponent>(rightPoint.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
                                 );
                                 rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
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
    static Node* InstantiateStartIsland(Node* parentNode, float locX, float locY, float locZ, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("BASE", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_2\\wyspa_duza_okragla_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
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
        pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
		pNewNodeOwner->radius = 1000.0f;
        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;

    }


    static Node* InstantiateNormalEnemy(Node* parentNode, float locX, float locY, float locZ, float scale, Node* pPlayer)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Enemy", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNode)
        );
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic1.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic2.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic3.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic4.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack1.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack2.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack3.ogg");

        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\basic.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(2.1f, 1.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::ENEMY);
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
    static Node* InstantiateFlyingEnemy(Node* parentNode, float locX, float locY, float locZ, float scale, Node* pPlayer)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Enemy", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNode)
        );
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic1.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic2.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic3.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic4.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack1.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack2.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack3.ogg");


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\flying.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(2.1f, 1.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::ENEMY);
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
            std::make_unique<Flying>(pNewNode)
        );
        pNewNode->AddComponent(
            std::make_unique<ShootAttack>(pNewNode)
        );
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        pNewNode->GetComponent<StateMachine>()->attackRange = 35.0f;
        pNewNode->GetComponent<StateMachine>()->followDistance = 60.0f;
        pNewNode->GetComponent<StateMachine>()->isFlying = true;
        pNewNode->GetComponent<StateMachine>()->pPlayer = pPlayer;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }
    static Node* InstantiateShootingEnemy(Node* parentNode, float locX, float locY, float locZ, float scale, Node* pPlayer)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Enemy", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNode)
        );
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic1.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic2.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic3.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic4.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack1.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack2.ogg");
        pNewNode->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\basic_attack3.ogg");


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\ranged.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(1.2f, 1.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::ENEMY);
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
   
    static Node* InstantiateBullet(float locX, float locY, float locZ, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Bullet", nullptr, "BULLET");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        root->AddChild(std::move(pNewNodeOwner));


        BodyCreationSettings BodySettings(new JPH::SphereShape(0.1f), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Kinematic, Layers::WALL);
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
        BodySettings.mMassPropertiesOverride.mMass = 0.1f;
        BodySettings.mFriction = 0.0f;
        BodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, BodySettings)
        );

        BodyCreationSettings trBodySettings(new JPH::SphereShape(0.7f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, trBodySettings, false)
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