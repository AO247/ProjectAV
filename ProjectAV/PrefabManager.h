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

    ///////////////////////////////
    ///////////ENVIROMENT//////////
    ///////////////////////////////

    static Node* InstantiateMushroom1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Mushroom1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\grzyb_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateMushroom2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Mushroom2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\grzyb_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateMushroom3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Mushroom3", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\grzyb_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateSpike1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Spike1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kolce_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateSpike2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Spike2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kolce_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateFaceColumn1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("FaceColumn1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\filar_twarze.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateFire1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Fire1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\krata_ogien.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    ///////////////////////////////
    ////////WALLS AND OBJECTS//////
    ///////////////////////////////

    static Node* InstantiateRock1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Rock1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\glazy_0606\\glaz_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRock2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Rock2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\glazy_0606\\glaz_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRock3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Rock3", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\glazy_0606\\glaz_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRock4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Rock4", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\glazy_0606\\glaz_4.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRock5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Rock5", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\glazy_0606\\glaz_5.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRockDouble(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("RockDouble", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\glazy_0606\\glaz_podwojny.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateLog1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Log1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\konar_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateLog2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Log2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\konar_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateWall1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Wall1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\murek_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateWall2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Wall2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\murek_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateWall3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Wall3", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\murek_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateWall4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Wall4", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\sciana_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuin1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ruin1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\ruina_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuin2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ruin2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\ruina_2_podstawa.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuin3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ruin3", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\ruina_2_top.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuin4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ruin4", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\ruina_duza_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuin5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ruin5", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\ruina_duza_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuinRoof1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("RuinRoof1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\sufit_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateRuinRoof2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("RuinRoof2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\sufit_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    /*static Node* InstantiateStatue(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Statue", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kolce_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList modelTriangles = PhysicsCommon::MakeTriangleList(model->GetAllTriangles());
        MeshShapeSettings modelMeshSettings(modelTriangles);
        Shape::ShapeResult modelMeshCreationResult = modelMeshSettings.Create();
        ShapeRefC modelMeshShape = modelMeshCreationResult.Get();
        ScaledShapeSettings modelScaling(modelMeshShape, Vec3Arg(scale, scale, scale));
        modelMeshShape = modelScaling.Create().Get();
        BodyCreationSettings bodySettings(modelMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }*/
    static Node* InstantiateColumn(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna_podwojna.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column3", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna_podwojna_z.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column4", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna_z.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column5", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna_z_podstawa.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn6(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column6", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna_z_srodek.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateColumn7(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Column7", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\kolumna_z_top.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    ///////////////////////////////
    ///////////THROWABLE///////////
    /////////////////////////////// 

    static Node* InstantiateStick1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stick1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\patyk_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStick2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stick2", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\patyk_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale/2, scale, scale));

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStone1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone1", nullptr, "STONE");

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
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStone2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone2", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStone3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone3", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStone4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone4", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_4.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStone5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone5", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_1\\kamien_5.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiatePot1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Pot1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiatePot3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Pot3", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiatePot4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Pot4", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male_2\\waza_4.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateBrick(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Brick", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_1\\cegla.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStoneStack1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("StoneStack1", nullptr, "STONE");
        
        InstantiateStoneStackBase(pNewNodeOwner.get(), Vector3(0.0f, 2.0f, 0.0f), 0.5f);
        Node* middle = InstantiateStoneStackMid(pNewNodeOwner.get(), Vector3(1.3f, 8.0f, -0.6f), 0.5f);
        Node* top = InstantiateStoneStackTop(pNewNodeOwner.get(), Vector3(1.3f, 12.0f, -0.2f), 0.5f);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStoneStackBase(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("StoneStackBase1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\stos_podstawa.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        ScaledShapeSettings islandScaling(islandShape, Vec3Arg(scale, scale, scale));
        islandShape = islandScaling.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 35.0f;
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStoneStackTop(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("StoneStackTop1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\stos_top.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        ScaledShapeSettings islandScaling(islandShape, Vec3Arg(scale, scale, scale));
        islandShape = islandScaling.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateStoneStackMid(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("StoneStackMid1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny_2\\stos_srodek.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        ScaledShapeSettings islandScaling(islandShape, Vec3Arg(scale, scale, scale));
        islandShape = islandScaling.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 14.0f;
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    ///////////////////////////////
    //////////ISLANDS PARTS////////
    ///////////////////////////////

    static Node* InstantiatePlatform1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_0606\\platforma_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );


        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiatePlatform2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Platform2", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_0606\\platforma_2.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );


        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiatePlatform3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {


        auto pNewNodeOwner = std::make_unique<Node>("Platform3", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_0606\\platforma_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );


        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiatePlatform4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Platform4", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_0606\\skala_1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );


        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiatePlatform5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
            auto pNewNodeOwner = std::make_unique<Node>("Platform5", nullptr, "GROUND");

            pNewNodeOwner->AddComponent(
                std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\platformy_0606\\skala_2.obj")
            );
            pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
            ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
            TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
            MeshShapeSettings islandMeshSettings(islandTriangles);
            Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
            ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
            ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
            islandMeshShape = islandScaling.Create().Get();
            BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
            bodySettings.mFriction = 1.0f;
            pNewNodeOwner->AddComponent(
                std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
            );


            pNewNodeOwner->SetLocalPosition(position);
            pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
            pNewNodeOwner->SetLocalRotation(rotation);

            Node* pNewNode = pNewNodeOwner.get();
            parentNode->AddChild(std::move(pNewNodeOwner));
            return pNewNode;
    }
    static Node* InstantiatePlatform6(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Platform6", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );


        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }

    ///////////////////////////////
    ////////////ISLANDS////////////
    ///////////////////////////////

    static Node* InstantiateFirstIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateColumn(pNewNode, Vector3(21.0f, 0.0f, 20.0f), 4.0f);
        InstantiateColumn(pNewNode, Vector3(22.0f, 0.0f, -19.0f), 4.0f);
        InstantiateColumn(pNewNode, Vector3(-14.0f, 0.0f, 20.0f), 4.0f);
        InstantiateColumn(pNewNode, Vector3(-14.0f, 0.0f, -19.0f), 4.0f);
		InstantiateBaseColumn(pNewNode, Vector3(0.0f, -14.0f, 0.0f), 4.0f);


        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateStartIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);
        pNewNodeOwner->radius = 1000.0f;

        /*InstantiateMushroom1(pNewNodeOwner.get(), 10.0f, 15.0f, 10.0f, 1.0f);
        InstantiateMushroom2(pNewNodeOwner.get(), 20.0f, 15.0f, 10.0f, 1.0f);
        InstantiateMushroom3(pNewNodeOwner.get(), 30.0f, 15.0f, 10.0f, 1.0f);
        InstantiateSpike1(pNewNodeOwner.get(), 40.0f, 15.0f, 10.0f, 1.0f);
        InstantiateSpike2(pNewNodeOwner.get(), 0.0f, 15.0f, 10.0f, 1.0f);
        InstantiateFaceColumn1(pNewNodeOwner.get(), -10.0f, 15.0f, 10.0f, 1.0f);
        InstantiateFire1(pNewNodeOwner.get(), -20.0f, 15.0f, 10.0f, 1.0f);*/


        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;

    }

    static Node* InstantiateIslandBig1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_0606\\wyspa_duza_o.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.0f, 4.0f, -1.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(66.0f, 0.0f, 15.0f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-10.0f, 0.0f, 60.0f));
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        //InstantiateStone1(pNewNode, 22.2f, 30.0f, 32.2f, 2.0f);
        //InstantiateStone1(pNewNode, 4.2f, 30.0f, 17.2f, 2.0f);
        InstantiateRock1(pNewNode, Vector3(-12.0f, 0.0f, 31.0f), 1.0f);
        InstantiateRock2(pNewNode, Vector3(30.0f, 0.0f, -41.0f), 1.0f);
        InstantiateStone1(pNewNode, Vector3(-30.0f, 4.0f, 20.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-10.0f, 4.0f, -21.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(16.0f, 4.0f, 30.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(33.0f, 4.0f, 22.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-8.0f, 4.0f, 12.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-24.0f, 4.0f, -31.0f), 2.0f);
        InstantiateNewColumn(pNewNode, Vector3(0.0f, 0.0f, 1.0f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(0.0f, 0.0f, -30.0f), 1.0f);
        InstantiateRuin1(pNewNode, Vector3(37.0f, -1.0f, -10.0f), 1.0f);
        InstantiatePlatform1(pNewNode, Vector3(-34.0f, -0.6f, -4.0f), 1.0f);
        //InstantiatePlatform3(pNewNode, -19.0f, 0.0f, -26.0f, 1.0f);
        //InstantiateStatue(pNewNode, 0.0f, 0.0f, 0.0f, 1.0f);


        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-19.0f, 8.0f, 2.0f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(8.0f, 8.0f, 8.0f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(6.0f, 8.0f, 24.0f));
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
    static Node* InstantiateIslandBig2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Island B2", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.0f, 3.0f, 14.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.0f, 0.0f, 0.0f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.0f, 3.0f, 58.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(4.0f, 0.0f, -64.0f));
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        //InstantiateStone1(pNewNode, 22.2f, 30.0f, 32.2f, 2.0f);
        //InstantiateStone1(pNewNode, 4.2f, 30.0f, 17.2f, 2.0f);
        InstantiateRock3(pNewNode, Vector3(40.0f, 0.0f, 27.0f), 1.0f);
        InstantiateRock5(pNewNode, Vector3(-38.0f, 0.0f, -4.0f), 1.0f);
        InstantiateStone1(pNewNode, Vector3(43.0f, 4.0f, -10.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-6.0f, 4.0f, -11.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(16.0f, 4.0f, 30.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(36.0f, 4.0f, -27.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(47.0f, 4.0f, 1.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-10.0f, 8.0f, 17.0f), 2.0f);
        InstantiateNewColumn(pNewNode, Vector3(33.0f, 0.0f, 10.0f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(-7.0f, 0.0f, -32.0f), 1.0f);
        InstantiateRuin3(pNewNode, Vector3(47.0f, 0.0f, -10.0f), 1.0f);
        InstantiateRuin3(pNewNode, Vector3(-16.0f, 0.0f, -27.0f), 1.0f);
        InstantiatePlatform2(pNewNode, Vector3(-11.0f, 0.0f, 19.0f), 1.0f);
        //InstantiatePlatform3(pNewNode, -19.0f, 0.0f, -26.0f, 1.0f);
        //InstantiateStatue(pNewNode, 0.0f, 0.0f, 0.0f, 1.0f);


        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(30.0f, 8.0f, -9.0f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(8.0f, 8.0f, -10.0f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(5.0f, 8.0f, -34.0f));
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
    static Node* InstantiateIslandBig3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B3", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(39.0f, 0.0f, 50.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -45.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-62.00f, 18.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(82.0f, 0.0f, -4.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
		rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(7.99f, 1.70f, -15.09f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-28.72f, 19.13f, -4.21f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(29.13f, 2.17f, 14.48f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(44.39f, 1.72f, -17.77f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-1.88f, 1.72f, 15.39f), 2.0f);
        InstantiateNewColumn(pNewNode, Vector3(52.40f, 0.00f, 18.61f), 1.0f);
        InstantiatePot4(pNewNode, Vector3(30.05f, 0.64f, -31.02f), 1.0f);
        InstantiateMushroom1(pNewNode, Vector3(9.30f, 0.00f, 20.30f), 0.4f, Vector3(-0.00f, 1.15f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-39.10f, 17.70f, -15.10f), 0.4f, Vector3(0.00f, -1.05f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-10.10f, 0.00f, -18.20f), 0.4f);
        InstantiateMushroom3(pNewNode, Vector3(-12.70f, 0.00f, -22.40f), 0.4f);
        InstantiatePlatform3(pNewNode, Vector3(-30.53f, 1.20f, -4.30f), 1.0f, Vector3(0.00f, -0.10f, 0.00f));
        InstantiateWall1(pNewNode, Vector3(20.20f, 0.00f, -37.60f), 1.0f, Vector3(0.00f, 1.68f, 0.0f));
        InstantiateWall2(pNewNode, Vector3(11.30f, 0.00f, 40.40f), 1.0f, Vector3(0.00f, 1.64f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
          std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(40.00f, 2.00f, -14.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-33.20f, 22.50f, 4.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(13.00f, 2.00f, 0.00f));
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
    static Node* InstantiateIslandBig4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B4", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(9.80f, 0.00f, 50.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-5.90f, 0.00f, -45.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-61.00f, 20.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(82.00f, 0.00f, -4.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(8.00f, 1.09f, -15.08f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-29.40f, 22.33f, -1.63f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(29.15f, 1.10f, 14.53f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(54.89f, 1.10f, -11.88f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-1.36f, 1.09f, 15.46f), 2.0f);
        InstantiateNewColumn(pNewNode, Vector3(21.00f, 0.00f, -23.89f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(52.40f, 0.00f, 18.61f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(-46.85f, 0.64f, -25.32f), 1.0f);
        InstantiateRock5(pNewNode, Vector3(-25.25f, 0.64f, -29.62f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateMushroom1(pNewNode, Vector3(46.20f, 0.00f, -1.60f), 0.4f, Vector3(-0.00f, 3.14f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-41.90f, 21.00f, -12.60f), 0.4f, Vector3(0.00f, -1.05f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-10.10f, 0.00f, -3.10f), 0.4f);
        InstantiateMushroom3(pNewNode, Vector3(-12.80f, 0.00f, -6.80f), 0.4f);
        InstantiatePlatform5(pNewNode, Vector3(-28.13f, 1.20f, 4.80f), 1.0f, Vector3(0.00f, 0.18f, 0.00f));
        InstantiateWall3(pNewNode, Vector3(32.80f, 0.00f, -27.90f), 1.0f, Vector3(0.00f, 1.26f, 0.0f));
        InstantiateWall3(pNewNode, Vector3(43.90f, 0.00f, 26.40f), 1.0f, Vector3(0.00f, -1.48f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(40.00f, 2.00f, -14.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-33.20f, 26.50f, 4.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(13.00f, 2.00f, 0.00f));
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
    static Node* InstantiateIslandBig5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B5", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(55.20f, 0.00f, 49.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-5.90f, 0.00f, -45.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-61.00f, 20.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(82.00f, 0.00f, -4.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(5.56f, 1.10f, -16.71f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-26.16f, 22.07f, -7.74f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(38.73f, 1.09f, 24.95f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(56.80f, 1.10f, -9.90f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-3.29f, 1.10f, 13.21f), 2.0f);
        InstantiateStoneStack1(pNewNode, Vector3(27.43f, 0.0f, 23.43f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(-46.85f, 0.64f, -25.32f), 1.0f);
        InstantiateRock5(pNewNode, Vector3(-17.35f, -0.26f, -29.62f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRockDouble(pNewNode, Vector3(41.15f, 0.64f, -25.82f), 1.0f, Vector3(0.00f, 1.54f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-46.85f, 0.64f, -25.32f), 1.0f);
        InstantiatePlatform5(pNewNode, Vector3(-28.13f, 1.20f, 4.80f), 1.0f, Vector3(0.00f, 0.18f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(25.70f, 0.90f, 2.20f), 1.0f, Vector3(-0.00f, 3.14f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(-39.60f, 21.00f, -12.60f), 1.0f, Vector3(0.00f, -1.05f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(-15.50f, 0.00f, -4.50f), 1.0f);
        InstantiateWall1(pNewNode, Vector3(10.60f, 0.00f, 42.20f), 1.0f, Vector3(0.00f, -1.48f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(49.90f, 2.00f, 6.80f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-33.20f, 26.50f, 1.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(3.10f, 2.00f, 0.00f));
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
    static Node* InstantiateIslandBig6(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B6", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.20f, 5.00f, 49.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-5.90f, 0.00f, -45.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-61.00f, 5.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(82.00f, 0.00f, -4.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(11.67f, 1.10f, -24.42f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-21.93f, 1.11f, -18.26f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(52.97f, 2.17f, 5.29f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(55.10f, 1.10f, -9.90f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-3.79f, 1.10f, 13.21f), 2.0f);
        InstantiateStoneStack1(pNewNode, Vector3(-14.67f, 0.00f, -9.17f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(25.53f, 0.00f, 0.93f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(-51.65f, 0.64f, -13.62f), 1.0f, Vector3(0.00f, 1.03f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(48.35f, -0.26f, 15.98f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRockDouble(pNewNode, Vector3(48.85f, 0.64f, -26.42f), 1.0f, Vector3(0.00f, -1.64f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-12.45f, 0.64f, 40.38f), 1.0f, Vector3(0.00f, -1.83f, 0.0f));
        InstantiateRock2(pNewNode, Vector3(-33.25f, 0.64f, 36.68f), 1.0f, Vector3(0.00f, -1.64f, 0.0f));
        InstantiatePlatform1(pNewNode, Vector3(-40.73f, 1.20f, 4.80f), 1.0f, Vector3(0.00f, 0.18f, 0.00f));
        InstantiatePlatform1(pNewNode, Vector3(-24.93f, 1.20f, 23.60f), 1.0f, Vector3(0.00f, 1.09f, 0.00f));
        InstantiatePlatform1(pNewNode, Vector3(1.77f, 1.20f, 28.80f), 1.0f, Vector3(0.00f, 1.37f, 0.00f));
        /*InstantiateSpike1(pNewNode, Vector3(-15.50f, 0.00f, -4.50f), 1.0f);
        InstantiateWall1(pNewNode, Vector3(10.60f, 0.00f, 42.20f), 1.0f, Vector3(0.00f, -1.48f, 0.0f));*/

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(45.00f, 2.00f, -0.90f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-16.40f, 2.90f, 5.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(7.60f, 2.00f, -7.50f));
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
    static Node* InstantiateIslandBig7(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B7", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.00f, 0.00f, -33.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -18.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 0.00f, 58.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-15.70f, 0.00f, -64.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(-15.70f, 1.19f, -6.30f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-29.33f, 1.19f, 9.04f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(27.08f, 2.26f, -7.62f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(42.50f, 1.19f, -13.20f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(35.80f, 1.20f, 21.18f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-0.37f, 1.81f, 17.97f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(3.77f, 13.25f, 0.87f), 2.0f);
        InstantiateWall2(pNewNode, Vector3(-41.25f, 0.64f, 9.48f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(51.65f, -0.26f, 19.68f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-43.75f, -0.26f, -21.72f), 1.0f, Vector3(0.00f, 0.68f, 0.00f));
        InstantiateWall1(pNewNode, Vector3(15.35f, 0.64f, -52.82f), 1.0f, Vector3(0.00f, -1.64f, 0.00f));
        InstantiateRuin5(pNewNode, Vector3(25.65f, 0.64f, 4.28f), 1.0f, Vector3(0.00f, -1.64f, 0.0f));
        InstantiateFire1(pNewNode, Vector3(23.57f, 1.20f, -26.70f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(-16.93f, 1.20f, -27.50f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(24.97f, 1.20f, 28.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(-17.23f, 1.20f, 27.30f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(41.80f, 2.00f, -27.80f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-17.50f, 2.90f, 5.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(4.20f, 2.00f, -25.70f));
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
    static Node* InstantiateIslandBig8(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B8", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.00f, 0.00f, -33.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -18.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 0.00f, 58.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-15.70f, 0.00f, -64.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(-15.70f, 1.19f, -6.30f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-29.33f, 1.19f, 9.04f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(27.08f, 2.26f, -7.62f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(42.50f, 1.19f, -13.20f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(35.80f, 1.20f, 21.18f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(10.60f, 5.84f, 1.45f), 2.0f);
        InstantiateWall1(pNewNode, Vector3(-41.25f, 0.64f, 9.48f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateWall2(pNewNode, Vector3(9.65f, 0.64f, -53.42f), 1.0f, Vector3(0.00f, -1.64f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(51.65f, -0.26f, 19.68f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-43.75f, -0.26f, -21.72f), 1.0f, Vector3(0.00f, 0.68f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(49.25f, -0.36f, 4.98f), 1.0f, Vector3(0.00f, 0.68f, 0.00f));
        InstantiateRuin3(pNewNode, Vector3(7.75f, 0.64f, 3.18f), 1.0f, Vector3(0.00f, -1.64f, 0.0f));
        InstantiateStoneStack1(pNewNode, Vector3(1.97f, 2.10f, 25.30f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(31.37f, 1.20f, -39.20f), 1.0f);
        InstantiateMushroom1(pNewNode, Vector3(23.57f, 1.20f, -26.70f), 0.4f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-16.93f, 1.20f, -27.50f), 0.4f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(24.97f, 1.20f, 28.80f), 0.4f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateMushroom3(pNewNode, Vector3(-17.23f, 1.20f, 27.30f), 0.4f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(7.70f, 2.00f, -38.80f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-17.50f, 2.90f, 5.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(4.70f, 2.00f, -15.00f));
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
    static Node* InstantiateIslandBig9(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B9", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.00f, 0.00f, -33.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -18.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 0.00f, 58.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-15.70f, 0.00f, -64.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(-27.64f, 1.20f, -28.02f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-28.03f, 1.19f, 15.14f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(8.40f, 1.20f, -47.59f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(46.30f, 1.19f, -18.50f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(26.39f, 1.19f, -39.83f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-41.25f, 1.20f, -1.77f), 2.0f);
        InstantiateWall3(pNewNode, Vector3(26.55f, 0.64f, 12.88f), 1.0f, Vector3(0.00f, -1.07f, 0.00f));
        InstantiateStoneStack1(pNewNode, Vector3(-0.13f, 2.10f, 30.00f), 1.0f);
		InstantiateNewColumn(pNewNode, Vector3(-15.03f, 1.20f, -40.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(-15.63f, 1.20f, 24.60f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(36.47f, 1.20f, -34.00f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateColumn2(pNewNode, Vector3(-32.23f, 1.20f, -8.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
		InstantiateBaseColumn(pNewNode, Vector3(7.67f, -44.40f, -8.50f), 12.0f, Vector3(0.00f, 0.0f, 0.00f));
		InstantiatePlatform3(pNewNode, Vector3(43.27f, 4.40f, 29.50f), 0.6f, Vector3(0.00f, 2.32f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(8.80f, 2.00f, -25.70f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-10.00f, 2.90f, -8.70f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(8.40f, 2.00f, 9.30f));
        spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(24.70f, 2.00f, -5.70f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
        pIsland->spawnPoints.push_back(spawnPoint4.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
        pNewNode->AddChild(std::move(spawnPoint4));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandBig10(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island B10", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.10f, 0.00f, 21.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -18.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(5.70f, 0.00f, 58.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.40f, 18.00f, -64.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(43.34f, 1.20f, -30.72f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-8.04f, 21.12f, -42.65f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(8.55f, 1.19f, -21.60f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(31.00f, 1.19f, 27.30f), 2.0f);
        InstantiateBrick(pNewNode, Vector3(-6.04f, 7.94f, 9.00f), 1.0f);
        InstantiateBrick(pNewNode, Vector3(43.35f, 0.50f, -3.65f), 1.0f);
		InstantiateRock2(pNewNode, Vector3(32.00f, 0.64f, -50.40f), 1.0f, Vector3(0.00f, -0.86f, 0.00f));
		InstantiateRock3(pNewNode, Vector3(-31.20f, 8.04f, 23.60f), 1.0f, Vector3(0.00f, -1.87f, -1.57f));
        InstantiateWall4(pNewNode, Vector3(1.17f, -0.60f, 28.40f), 1.0f, Vector3(0.00f, -1.57f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(-23.13f, 1.20f, -18.10f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(11.47f, 1.20f, -32.60f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiatePlatform2(pNewNode, Vector3(-9.23f, -0.20f, 20.30f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiatePlatform4(pNewNode, Vector3(-13.43f, -0.80f, -25.10f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(20.60f, 2.00f, -21.70f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(39.20f, 2.90f, 14.60f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(-1.10f, 10.20f, 23.80f));
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

    static Node* InstantiateIslandMedium1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M1", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-6.0f, 0.0f, 42.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(8.0f, 0.0f, -36.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-44.00f, 0.00f, 21.10f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(46.0f, 0.0f, 18.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(9.28f, 0.65f, 15.50f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(24.69f, 1.71f, 22.19f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(20.44f, 1.30f, -14.47f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-10.99f, 1.30f, -6.94f), 2.0f);
		InstantiateStoneStack1(pNewNode, Vector3(25.00f, 1.10f, 3.30f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(-7.20f, 1.10f, -14.00f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(-4.90f, 1.10f, 20.40f), 1.0f);
		InstantiateRock1(pNewNode, Vector3(-26.10f, 0.64f, -11.00f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-30.50f, 0.64f, 14.50f), 1.0f, Vector3(0.00f, 2.44f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(32.00f, 0.64f, -23.10f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-33.70f, 0.64f, 3.50f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(10.70f, 2.00f, -2.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-13.30f, 2.90f, 6.60f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandMedium2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M2", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-4.0f, 0.0f, 43.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, -36.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-44.0f, 0.0f, 2.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(46.0f, 0.0f, 10.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
        upPoint->AddComponent(
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStone1(pNewNode, Vector3(9.28f, 0.65f, 15.50f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(24.69f, 1.71f, 22.19f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(20.44f, 1.30f, -14.47f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-10.99f, 1.30f, -6.94f), 2.0f);
        InstantiateStoneStack1(pNewNode, Vector3(25.00f, 1.10f, 3.30f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(-7.20f, 1.10f, -14.00f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(-4.90f, 1.10f, 20.40f), 1.0f);
        InstantiateRock1(pNewNode, Vector3(-26.10f, 0.64f, -11.00f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-30.50f, 0.64f, 14.50f), 1.0f, Vector3(0.00f, 2.44f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(32.00f, 0.64f, -23.10f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-33.70f, 0.64f, 3.50f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(10.70f, 2.00f, -2.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-13.30f, 2.90f, 6.60f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }

    static Node* InstantiateIslandSmall1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-10.0f, 2.0f, 33.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -33.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-34.0f, 1.0f, 5.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(34.0f, 0.0f, -10.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStick1(pNewNode, Vector3(-14.0f, 15.0f, -15.0f), 0.5f);
        InstantiateStick2(pNewNode, Vector3(15.2f, 15.0f, -19.2f), 1.0f);
        InstantiateStone1(pNewNode, Vector3(-9.2f, 15.0f, 16.2f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(19.2f, 15.0f, -5.2f), 2.0f);
        //InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
        //InstantiateColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
        InstantiatePlatform2(pNewNode, Vector3(-7.2f, 0.0f, 7.7f), 0.75f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 2.0f, -4.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandSmall2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {


        auto pNewNodeOwner = std::make_unique<Node>("Island S2", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy_0606\\wyspa_mala_L.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(15.0f, 0.0f, 36.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -25.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 6.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, 16.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, Vector3(-8.0f, 30.0f, -9.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(18.2f, 30.0f, 3.2f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(17.2f, 30.0f, 22.2f), 2.0f);
        //InstantiateStone1(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
        InstantiateStone1(pNewNode, Vector3(-13.2f, 30.0f, -2.2f), 2.0f);
        InstantiateNewColumn(pNewNode, Vector3(12.0f, 0.0f, -5.0f), 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(1.0f, 2.0f, -1.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateIslandSmall3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S3", nullptr, "GROUND");

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
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 31.0f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -31.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 8.0f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -14.0f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        //Node* stone1 = InstantiateStone1(pNewNode, -10.2f, 15.0f, 8.2f, 2.0f);
        InstantiateStone1(pNewNode, Vector3(-9.0f, 15.0f, -9.0f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(8.2f, 15.0f, -10.2f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(-9.2f, 15.0f, 9.2f), 2.0f);
        InstantiateStone1(pNewNode, Vector3(8.2f, 15.0f, 9.2f), 2.0f);
        InstantiateNewColumn(pNewNode, Vector3(-14.5f, 0.0f, 15.0f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(14.5f, 0.0f, -15.0f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(14.5f, 0.0f, 15.0f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(-14.5f, 0.0f, -15.0f), 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 2.0f, 0.0f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pNewNode->AddChild(std::move(spawnPoint1));

        pNewNode->AddChild(std::move(leftPoint));
        pNewNode->AddChild(std::move(rightPoint));
        pNewNode->AddChild(std::move(upPoint));
        pNewNode->AddChild(std::move(downPoint));
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }

    static Node* InstantiateTopColumn(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Top", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\objects\\kolumna_1_top.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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


        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateMiddleColumn(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Middle", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\objects\\kolumna_1_srodek.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
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
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateBaseColumn(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Base", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\objects\\kolumna_1_podstawa.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
        MeshShapeSettings islandMeshSettings(islandTriangles);
        Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
        ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateNewColumn(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("New Column", nullptr, "WALL");
        Node* pNewNode = pNewNodeOwner.get();

        InstantiateBaseColumn(pNewNode, Vector3(0.0f, 2.0f, 0.0f), 1.0f);
        Node* middle = InstantiateMiddleColumn(pNewNode, Vector3(0.0f, 12.6f, 0.0f), 1.0f);
        Node* top = InstantiateTopColumn(middle, Vector3(0.0f, 7.9f, 0.0f), 1.0f);
        top->GetComponent<Rigidbody>()->ConnectWithOtherBody(middle->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 7.9f, 0.0f));

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    ///////////////////////////////
    ////////////ENEMIES////////////
    ///////////////////////////////

    static Node* InstantiateNormalEnemy(Node* parentNode, Vector3 position, float scale, Node* pPlayer)
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
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 1.5f;
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
        

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));



        return pNewNode;
    }
    static Node* InstantiateFlyingEnemy(Node* parentNode, Vector3 position, float scale, Node* pPlayer)
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

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }
    static Node* InstantiateShootingEnemy(Node* parentNode, Vector3 position, float scale, Node* pPlayer)
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
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 1.5f;
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

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }
   
    static Node* InstantiateHealthCollectable(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Health", nullptr, "COLLECTABLE");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        BodyCreationSettings bodySettings(new JPH::SphereShape(1.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, bodySettings, false)
        );
        pNewNode->AddComponent(
            std::make_unique<Collectable>(pNewNode)
        );
        pNewNode->GetComponent<Collectable>()->health = true;

        pNewNode->SetLocalPosition(position);
		pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateExpCollectable(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Exp", nullptr, "COLLECTABLE");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        BodyCreationSettings bodySettings(new JPH::SphereShape(1.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, bodySettings, false)
        );
        pNewNode->AddComponent(
            std::make_unique<Collectable>(pNewNode)
        );
        //pNewNode->GetComponent<Collectable>()->health = false;

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateBullet(Vector3 position, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Bullet", nullptr, "BULLET");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        root->AddChild(std::move(pNewNodeOwner));


        BodyCreationSettings BodySettings(new JPH::SphereShape(0.1f), RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
        BodySettings.mMassPropertiesOverride.mMass = 0.1f;
        BodySettings.mFriction = 0.0f;
        BodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, BodySettings)
        );
        PhysicsCommon::physicsSystem->GetBodyInterface().SetGravityFactor(pNewNode->GetComponent<Rigidbody>()->GetBodyID(), 0);

        BodyCreationSettings trBodySettings(new JPH::SphereShape(0.7f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, trBodySettings, false)
        );

        pNewNode->AddComponent(
            std::make_unique<Bullet>(pNewNode)
        );

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        return pNewNode;
    }

    Window* wnd;
private:
};