#pragma once
#include "Node.h"
#include <string>
#include "Components.h"
#include "PhysicsCommon.h"
#include "MainRenderGraph.h"
#include "Window.h"
#include "AnimationComponent.h"
#include "ParticleSystemComponent.h"
#include "TrailEmitterLogic.h"
#include "PointEmitterLogic.h"
#include "CircleEmitterLogic.h"
#include "SphereToCenterEmitterLogic.h"
#include "ModelCache.h" // Potrzebujemy dostêpu do naszego cache'u
#include "ModelException.h" // Potrzebujemy naszego typu wyj¹tku
#include <filesystem> // <-- Do³¹cz ten nag³ówek
#include <iostream>
#include "SphereVolumeEmitterLogic.h"

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


    
    static void PreloadAllModels()
    {
        namespace fs = std::filesystem;
        std::vector<std::string> modelsToPreload;
        const std::string rootDirectory = "Models";
        const std::vector<std::string> allowedExtensions = {
       ".obj", ".fbx", ".gltf", ".glb"
        };

        try
        {
            // SprawdŸ, czy katalog "Models" istnieje
            if (!fs::exists(rootDirectory) || !fs::is_directory(rootDirectory))
            {
                std::string errorMsg = "ERROR: Preloading directory not found: " + rootDirectory + "\n";
                OutputDebugStringA(errorMsg.c_str());
                return;
            }

            // Rekursywnie przeszukaj katalog "Models" i wszystkie jego podkatalogi
            for (const auto& entry : fs::recursive_directory_iterator(rootDirectory))
            {
                if (entry.is_regular_file())
                {
                    const fs::path& path = entry.path();
                    std::string extension = path.extension().string();

                    // SprawdŸ, czy rozszerzenie pliku jest na naszej liœcie
                    for (const auto& allowedExt : allowedExtensions)
                    {
                        // Porównanie bez uwzglêdniania wielkoœci liter (opcjonalne, ale dobre)
                        if (_stricmp(extension.c_str(), allowedExt.c_str()) == 0)
                        {
                            // Zapisz œcie¿kê w formacie, którego u¿ywasz (z podwójnymi backslashami)
                            std::string modelPathStr = path.string();
                            // std::filesystem zwraca œcie¿ki z pojedynczymi backslashami, które s¹ OK
                            // dla wiêkszoœci funkcji, ale jeœli potrzebujesz podwójnych, musisz je zamieniæ.
                            // Zak³adaj¹c, ¿e pojedyncze s¹ w porz¹dku, po prostu dodajemy.
                            modelsToPreload.push_back(modelPathStr);

                            // Zaloguj znaleziony model (do debugowania)
                            std::string logMsg = "Found model to preload: " + modelPathStr + "\n";
                            OutputDebugStringA(logMsg.c_str());
                            break; // Znaleziono pasuj¹ce rozszerzenie, przejdŸ do nastêpnego pliku
                        }
                    }
                }
            }
        }
        catch (const fs::filesystem_error& e)
        {
            std::string errorMsg = "Filesystem error during preloading: " + std::string(e.what()) + "\n";
            OutputDebugStringA(errorMsg.c_str());
            return;
        }

        
        OutputDebugStringA("--- Preloading GPU assets... ---\n");

        for (const auto& modelPath : modelsToPreload)
        {
            auto dummyOwner = std::make_unique<Node>("DummyPreloadNode");
            try
            {
                const auto& cachedData = ModelCache::Get().LoadOrGet(modelPath);
                bool hasBones = cachedData.BoneCounter > 0;
                dummyOwner->AddComponent(
                    std::make_unique<ModelComponent>(dummyOwner.get(), wind->Gfx(), modelPath, 1.0f, hasBones)
				);
            }
            catch (const std::exception& e) // U¿yj ogólniejszego wyj¹tku
            {
                std::string errorMsg = "ERROR preloading model (GPU phase): " + modelPath + "\n" + e.what() + "\n";
                OutputDebugStringA(errorMsg.c_str());
            }
        }

        OutputDebugStringA("--- All preloading finished. ---\n");
    }
#pragma region  ENVIROMENT
    
    ///////////////////////////////
    ///////////ENVIROMENT//////////
    ///////////////////////////////
    static Node* InstantiateMushroom1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Mushroom1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\grzyb_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\grzyb_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\grzyb_3.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\kolce.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\filar_grzyby.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\krata.obj")
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

    static Node* InstantiateBush1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Bush1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\dekoracje\\1.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        /*ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
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
        );*/

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateBush2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Bush2", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\dekoracje\\2.fbx")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        /*ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
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
        );*/

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateBush3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Bush3", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\dekoracje\\3.fbx")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        /*ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
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
        );*/

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateBush4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Bush4", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\dekoracje\\4.fbx")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        /*ModelComponent* model = pNewNodeOwner->GetComponent<ModelComponent>();
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
        );*/

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
#pragma endregion

#pragma region WALLS AND OBJECTS

    ///////////////////////////////
    ////////WALLS AND OBJECTS//////
    ///////////////////////////////

    static Node* InstantiateRock1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Rock1", nullptr, "WALL");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\glaz_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\glaz_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\glaz_3.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\glaz_4.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\glaz_5.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\glaz_podwojny.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\konar_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\konar_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\murek_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\murek_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\murek_3.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\sciana_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\ruina_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\ruina_2_podstawa.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\ruina_2_top.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\ruina_duza_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna_duza_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\sufit_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\sufit_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna_podwojna.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna_podwojna_z.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna_z_podstawa.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna_z_srodek.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\kolumna_z_top.obj")
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
#pragma endregion

#pragma region THROWABLE



    ///////////////////////////////
    ///////////THROWABLE///////////
    /////////////////////////////// 

    static Node* InstantiateStick1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stick1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\patyk_1.obj")
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
    static Node* InstantiateStick2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stick2", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\patyk_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\kamien_1.obj")
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
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\kamien_2.obj")
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
    static Node* InstantiateStone3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone3", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\kamien_3.obj")
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
    static Node* InstantiateStone4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone4", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\kamien_4.obj")
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
    static Node* InstantiateStone5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Stone5", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<SoundEffectsPlayer>(pNewNodeOwner.get())
        );
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit1.ogg");
        pNewNodeOwner->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\rock_hit2.ogg");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\kamien_5.obj")
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
    static Node* InstantiatePot1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Pot1", nullptr, "STONE");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\waza_1.obj")
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
        pNewNodeOwner->GetComponent<Throwable>()->pot = true;
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\waza_3.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
        ConvexHullShapeSettings shapeSettings(PhysicsCommon::MakeVertexArray(islandModel->GetAllUniqueVertices()));
        ShapeRefC islandShape = shapeSettings.Create().Get();
        ScaledShapeSettings islandScaling(islandShape, Vec3Arg(scale, scale, scale));
        islandShape = islandScaling.Create().Get();
        BodyCreationSettings BodySettings(islandShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
        BodySettings.mMassPropertiesOverride.mMass = 10.0f;
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
        pNewNodeOwner->GetComponent<Throwable>()->pot = true;

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\waza_4.obj")
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
        pNewNodeOwner->GetComponent<Throwable>()->pot = true;

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\ruiny\\cegla.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\stos_podstawa.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\stos_top.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\enviro_male\\stos_srodek.obj")
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
#pragma endregion

#pragma region ISLANDS PARTS



    ///////////////////////////////
    //////////ISLANDS PARTS////////
    ///////////////////////////////

    static Node* InstantiatePlatform1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\platforma_1.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\platforma_2.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\platforma_3.obj")
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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\skala_1.obj")
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
                std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\skaly\\skala_2.obj")
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
#pragma endregion

#pragma region ISLANDS



    ///////////////////////////////
    ////////////ISLANDS////////////
    ///////////////////////////////

    static Node* InstantiateFirstIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Spawn", nullptr, "SPAWN");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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
		InstantiateBaseColumn(pNewNode, Vector3(0.00f, -3.10f, 0.00f), 4.0f, 1.0f);
        //InstantiateAnimationnTest(pNewNode, Vector3(0.0f, 2.0f, 0.0f), 1.0f);
        //InstantiateBaseColumn(pNewNode, Vector3(0.0f, 2.0f, 0.0f), 4.0f, 1.0f);

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_duza_o.obj")
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

    static Node* InstantiateAbilityIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("AbilityIsland", nullptr, "ABILITY");
        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);
        Node* pNewNode = pNewNodeOwner.get();


        InstantiateRock1(pNewNode, Vector3(92.00f, -2.80f, -31.90f), 0.6f, Vector3(0.00f, 0.00f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(13.30f, 0.00f, -44.70f), 1.0f, Vector3(0.00f, 0.00f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(51.10f, -0.10f, -36.00f), 2.0f, Vector3(0.00f, -0.89f, 3.14f));
        InstantiateRock1(pNewNode, { 173.30f, 0.00f, 2.20f }, 0.6f, Vector3(0.00f, 0.00f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(138.10f, 0.10f, -31.70f), 2.0f, Vector3(0.00f, -1.45f, 3.14f));
        InstantiateRock2(pNewNode, Vector3(22.00f, 0.50f, -15.20f), 1.0f, Vector3(0.0f, -0.19f, 0.0f));
        InstantiateRock4(pNewNode, Vector3(-15.90f, 0.00f, -7.40f), 1.0f, Vector3(0.00f, -1.29f, 0.00f));
        InstantiateRock1(pNewNode, { -6.06f, 16.70f, 35.21f }, 0.9f, Vector3(0.00f, 0.00f, 3.14f));
		InstantiateBaseColumn(pNewNode, { 66.9f, -1.4f, -31.9f }, 0.8f, 1.0f);
        InstantiatePlatform3(pNewNode, Vector3(0.00f, 0.00f, 8.30f), 1.0f, Vector3(0.00f, 1.75f, 0.00f));

        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateJumpIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("JumpIsland", nullptr, "ABILITY");
        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);
        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStoneStack1(pNewNode, Vector3(-7.22f, 2.22f, 2.98f), 1.0f);
        InstantiateRock2(pNewNode, Vector3(21.70f, -0.10f, -17.70f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(6.40f, 0.20f, 22.50f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(22.30f, 0.00f, -10.90f), 1.0f);
        InstantiateRock4(pNewNode, Vector3(-2.10f, -2.40f, 25.50f), 1.0f, Vector3(0.0f, -0.86f, 0.0f));
        InstantiateRock5(pNewNode, Vector3(27.10f, -0.90f, 0.10f), 1.0f);
		InstantiateRock5(pNewNode, Vector3(11.80f, -1.80f, -17.20f), 0.8f, Vector3(0.00f, 0.00f, -0.31f));
		InstantiateRock1(pNewNode, Vector3(1.30f, -0.60f, -14.30f), 0.8f, Vector3(0.00f, -2.01f, 0.00f));
        InstantiateRockDouble(pNewNode, Vector3(-12.90f, -0.30f, -13.10f), 0.6f, Vector3(0.00f, 2.84f, 0.0f));
        InstantiateRockDouble(pNewNode, Vector3(19.50f, 0.00f, 16.10f), 1.0f, Vector3(0.00f, 1.75f, 0.00f));

        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateDoubleJumpIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("DoubleJumpIsland", nullptr, "ABILITY");
        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_duza_p.obj")
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);
        Node* pNewNode = pNewNodeOwner.get();

        InstantiateStoneStack1(pNewNode, Vector3(-18.12f, 1.72f, 25.88f), 1.0f);
        InstantiateRock1(pNewNode, Vector3(27.40f, 3.00f, -3.80f), 1.0f, Vector3(0.00f, 0.00f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(8.60f, 8.90f, 12.60f), 1.0f, Vector3(0.00f, -1.22f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(-13.90f, 13.30f, -2.90f), 1.0f, Vector3(0.00f, 1.45f, 3.14f));
        InstantiateRock1(pNewNode, { -55.20f, 19.00f, -13.90f }, 1.0f, {-0.00f, 0.18f, -3.14f});
        InstantiateRock2(pNewNode, Vector3(66.00f, -0.10f, -12.90f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(61.20f, 0.20f, -3.00f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(33.90f, 0.00f, -32.80f), 1.0f);
        InstantiateRock4(pNewNode, Vector3(25.60f, -8.60f, 36.10f), 1.0f, Vector3(0.0f, -2.85f, 0.0f));
        InstantiateRock4(pNewNode, Vector3(-41.80f, -3.80f, 25.50f), 1.0f, Vector3(0.0f, -0.86f, 0.0f));
        InstantiateRock4(pNewNode, Vector3(9.60f, -5.30f, 37.90f), 1.0f, Vector3(0.0f, -2.29f, 0.0f));
        InstantiateRock5(pNewNode, Vector3(22.60f, -0.90f, -30.20f), 1.0f);
        InstantiateRockDouble(pNewNode, Vector3(-17.30f, -0.30f, -24.00f), 0.6f, Vector3(0.00f, 2.86f, 0.0f));
        InstantiateRockDouble(pNewNode, Vector3(60.10f, 0.00f, 18.00f), 1.0f, Vector3(0.00f, 0.96f, 0.00f));

        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateUltIsland(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("UltIsland", nullptr, "ABILITY");
        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);
        Node* pNewNode = pNewNodeOwner.get();

		InstantiateThrowable(pNewNode, Vector3(-3.84f, 0.66f, 14.90f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-17.13f, 0.64f, -14.36f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(8.31f, 0.64f, -5.58f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(-7.92f, 1.72f, -11.22f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(-13.62f, 0.0f, 9.78f), 1.0f);
        InstantiateRock1(pNewNode, Vector3(-6.90f, 5.20f, 28.00f), 1.0f, Vector3(0.00f, 0.63f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(-7.40f, 13.60f, 50.30f), 1.0f, Vector3(0.00f, 1.38f, 3.14f));
        InstantiateRock1(pNewNode, Vector3(-12.80f, 21.30f, 78.10f), 1.0f, Vector3(0.00f, 0.63f, 3.14f));
        InstantiateRock3(pNewNode, Vector3(32.70f, -0.30f, -9.80f), 1.0f);
        InstantiateRock4(pNewNode, Vector3(-26.00f, -0.30f, 23.70f), 1.0f);
        InstantiateRock5(pNewNode, Vector3(30.40f, -0.40f, 0.60f), 1.0f);
        InstantiateRockDouble(pNewNode, Vector3(24.10f, -0.40f, 19.40f), 1.0f, Vector3(0.00f, 1.10f, 0.00f));

		parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }
    static Node* InstantiateTutorialIslands(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Tutorial", nullptr, "GROUND");

        pNewNodeOwner->SetLocalPosition({ 185.10f, -19.50f, -448.20f });
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation({ 0.00f, 1.54f, 0.00f });

        pNewNodeOwner->AddComponent(
            std::make_unique<Tutorial>(pNewNodeOwner.get(), wind, player)
        );
        Tutorial* tut = pNewNodeOwner->GetComponent<Tutorial>();

        Node* JumpIsland = InstantiateJumpIsland(pNewNodeOwner.get(), { -15.0f, 0.0f, -169.0f }, 1.0f, { 0.00f, 1.57f, 0.0f });
        Node* DoubleJumpIsland = InstantiateDoubleJumpIsland(pNewNodeOwner.get(), { 3.0f, -2.0f, -68.0f }, 1.0f, { 0.00f, 1.01f, 0.00f });
		Node* AbilityIsland = InstantiateAbilityIsland(pNewNodeOwner.get(), { -92.29f, 0.00f, 50.61f }, 1.0f, { 0.0f, 2.36f, 0.0f });
		Node* UltIsland = InstantiateUltIsland(pNewNodeOwner.get(), { -250.91f, -0.01f, -137.29f }, 1.0f, { 0.0f, -1.66f, 0.04f });
        
        tut->stone1 = InstantiateStone1(pNewNodeOwner.get(), { -163.0f, 3.2f, 26.0f }, 0.4f);
        tut->stone2 = InstantiateStone1(pNewNodeOwner.get(), { -210.0f, 1.8f, -40.0f }, 0.4f);
        Node* enemy1 = InstantiateNormalEnemy(root, { 187.4f, -18.3f, -267.6f }, 1.6f);
        Node* enemy2 = InstantiateNormalEnemy(root, { 105.0f, -15.5f, -236.4f }, 1.6f);
        Node* enemy3 = InstantiateNormalEnemy(root, { 44.1f, -15.3f, -198.9f }, 1.6f);
        Node* enemy4 = InstantiateShootingEnemy(root, { 32.2f, -15.8f, -203.6f }, 1.6f);
        Node* enemy5 = InstantiateShootingEnemy(root, { 35.2f, -15.8f, -190.6f }, 1.6f);

        enemy1->GetComponent<Walking>()->maxSpeed = 0.0f;
        enemy2->GetComponent<Walking>()->maxSpeed = 0.0f;
        enemy3->GetComponent<Walking>()->maxSpeed = 0.0f;        
        enemy4->GetComponent<Walking>()->maxSpeed = 0.0f;
        enemy5->GetComponent<Walking>()->maxSpeed = 0.0f;

        tut->SetStones();
		//InstantiateNormalEnemy(pNewNodeOwner.get(), Vector3(0.0f, 0.0f, 0.0f), 1.0f, pPlayer);
		//InstantiateNormalEnemy(pNewNodeOwner.get(), Vector3(0.0f, 0.0f, 0.0f), 1.0f, pPlayer);



        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));
        return pNewNode;
    }

    static Node* InstantiateIslandBig1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspy_duza_o.obj")
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-53.90f, 5.20f, -1.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(66.00f, 0.00f, 15.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-10.00f, 0.00f, 60.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-2.20f, 0.00f, -55.30f));
        pIsland->halfExtents = { 63.0f, 2.0f, 63.0f };
        /*pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };*/
                               /* upPoint->AddComponent(
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

        InstantiateRock1(pNewNode, Vector3(-12.0f, 0.0f, 31.0f), 1.0f);
        InstantiateRock2(pNewNode, Vector3(32.50f, 0.00f, -44.10f), 1.0f);
        InstantiateThrowable(pNewNode, Vector3(-30.0f, 4.0f, 20.0f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-10.0f, 4.0f, -21.0f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(16.00f, 1.19f, 30.00f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(36.79f, 1.19f, 33.33f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-8.0f, 4.0f, 12.0f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-24.0f, 4.0f, -31.0f), 0.4f);
        InstantiateNewColumn(pNewNode, Vector3(0.00f, 0.00f, 7.40f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(0.00f, 0.00f, -25.60f), 1.0f);
        InstantiateRuin1(pNewNode, Vector3(39.90f, -1.00f, -5.90f), 1.0f);
        InstantiatePlatform1(pNewNode, Vector3(-34.90f, -0.60f, -4.40f), 1.0f, Vector3(0.00f, -0.17f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        //spawnPoint1->AddComponent(
        //    std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        //spawnPoint2->AddComponent(
        //    std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        //spawnPoint3->AddComponent(
        //    std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        //spawnPoint4->AddComponent(
        //    std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        //spawnPoint5->AddComponent(
        //    std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        //spawnPoint6->AddComponent(
        //    std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        //);
        //spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-19.0f, 3.0f, 2.0f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(11.90f, 3.0f, 8.50f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(7.10f, 3.0f, 26.30f));
        spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(32.90f, 3.0f, 19.20f));
        spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(-1.50f, 3.0f, -10.00f));
        spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(15.40f, 3.0f, -29.60f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));



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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspy_duza_o.obj")
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.00f, 3.00f, -9.90f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, 6.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 10.50f, 60.70f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(4.00f, 0.00f, -58.00f));
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

        InstantiateRock2(pNewNode, Vector3(-30.20f, 0.00f, 33.40f), 1.5f);
        InstantiateRock2(pNewNode, Vector3(-37.00f, 0.00f, 21.00f), 1.2f, Vector3(0.0f, 1.57f, 0.0f));
        InstantiateRock3(pNewNode, Vector3(27.30f, 0.00f, -30.70f), 1.0f);
        InstantiateRock5(pNewNode, Vector3(-36.40f, 0.00f, 10.20f), 1.0f);
        InstantiateThrowable(pNewNode, Vector3(16.72f, 1.19f, 13.86f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-10.17f, 1.21f, -5.14f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-9.81f, 9.06f, 38.49f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(38.40f, 1.19f, -21.00f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(47.52f, 1.20f, 23.94f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-24.47f, 9.05f, 17.28f), 0.4f);
        InstantiateNewColumn(pNewNode, Vector3(33.00f, 0.00f, 16.00f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(-7.00f, 0.00f, -26.00f), 1.0f);
        InstantiateRuin3(pNewNode, Vector3(47.0f, 0.0f, -4.0f), 1.0f, Vector3(0.00f, 0.59f, 0.00f));
        InstantiateRuin3(pNewNode, Vector3(-16.0f, 0.0f, -21.0f), 1.0f);
        InstantiatePlatform2(pNewNode, Vector3(-8.40f, 0.00f, 28.80f), 1.0f);


        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(31.10f, 3.50f, -3.00f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(11.80f, 3.50f, -4.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(11.40f, 3.50f, -28.00f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(-26.30f, 3.50f, -14.00f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(-6.50f, 8.00f, 20.10f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(-20.50f, 8.00f, 35.60f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));



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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_duza_p.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(47.70f, 0.00f, 48.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -42.60f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-58.50f, 18.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(81.00f, 0.00f, -4.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(7.99f, 1.70f, -15.09f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-28.72f, 19.13f, -4.21f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(29.13f, 2.17f, 14.48f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(44.39f, 1.72f, -17.77f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-1.88f, 1.72f, 15.39f), 0.4f);
        InstantiateRock3(pNewNode, Vector3(-26.10f, 0.00f, -31.90f), 0.7f);
        InstantiateRock3(pNewNode, Vector3(-43.60f, 0.00f, 25.60f), 0.6f, Vector3(0.00f, -1.22f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(52.40f, 0.00f, 18.61f), 1.0f);
        InstantiatePot4(pNewNode, Vector3(30.05f, 0.64f, -31.02f), 1.0f);
        InstantiateMushroom1(pNewNode, Vector3(30.90f, 0.00f, 25.20f), 0.4f, Vector3(-0.00f, 1.15f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-39.10f, 17.70f, -15.10f), 0.4f, Vector3(0.00f, -1.05f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-10.10f, 0.00f, -18.20f), 0.4f);
        InstantiateMushroom3(pNewNode, Vector3(-12.70f, 0.00f, -22.40f), 0.4f);
        InstantiatePlatform3(pNewNode, Vector3(-30.53f, 0.40f, -4.30f), 1.0f, Vector3(0.00f, -0.10f, 0.00f));
        InstantiateWall1(pNewNode, Vector3(19.10f, 0.00f, -34.20f), 1.0f, Vector3(0.00f, 1.68f, 0.0f));
        InstantiateWall2(pNewNode, Vector3(8.90f, 0.00f, 43.30f), 1.0f, Vector3(0.00f, 1.64f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(30.40f, 2.00f, -14.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-33.40f, 22.50f, 4.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(11.80f, 2.00f, 0.00f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(9.50f, 2.00f, 26.20f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(43.10f, 2.00f, 3.60f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(-13.00f, 2.00f, 29.00f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_duza_p.obj")
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
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-5.10f, 0.00f, -42.20f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.40f, 20.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(82.00f, 0.00f, -4.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        //pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
        //                        2.0f,
        //                        (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
       /* upPoint->AddComponent(
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

        InstantiateThrowable(pNewNode, Vector3(8.00f, 1.09f, -15.08f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-29.40f, 22.33f, -1.63f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(29.15f, 1.10f, 14.53f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(54.89f, 1.10f, -11.88f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-1.36f, 1.09f, 15.46f), 0.4f);
        InstantiateNewColumn(pNewNode, Vector3(21.00f, 0.00f, -23.89f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(52.40f, 0.00f, 18.61f), 1.0f);
		InstantiateRock1(pNewNode, Vector3(18.60f, -0.60f, 38.30f), 0.6f, Vector3(0.00f, 1.13f, -0.00f));
		InstantiateRock2(pNewNode, Vector3(61.60f, -1.00f, -23.20f), 0.8f, Vector3(-0.26f, -1.51f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-47.95f, 0.34f, -23.22f), 1.0f);
        InstantiateRock5(pNewNode, Vector3(-21.45f, -0.06f, -28.62f), 1.0f, Vector3(0.00f, -0.04f, 0.00f));
        InstantiateMushroom1(pNewNode, Vector3(46.20f, 0.00f, -1.60f), 0.4f, Vector3(-0.00f, 3.14f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-41.90f, 21.00f, -12.60f), 0.4f, Vector3(0.00f, -1.05f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-10.10f, 0.00f, -3.10f), 0.4f);
        InstantiateMushroom3(pNewNode, Vector3(-12.80f, 0.00f, -6.80f), 0.4f);
        InstantiatePlatform5(pNewNode, Vector3(-34.83f, 1.20f, 5.80f), 1.0f, Vector3(0.00f, 0.18f, 0.00f));
        InstantiateWall3(pNewNode, Vector3(32.60f, 0.00f, -29.90f), 1.0f, Vector3(0.00f, 1.41f, 0.0f));
        InstantiateWall3(pNewNode, Vector3(46.70f, 0.00f, 31.90f), 1.0f, Vector3(0.00f, -1.29f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
   /*     spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(40.00f, 2.00f, -14.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-33.20f, 26.50f, 4.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(13.00f, 2.00f, 0.00f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(13.30f, 2.00f, 26.20f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(37.80f, 2.00f, 24.20f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(-9.10f, 2.00f, 11.20f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_duza_p.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(55.20f, 0.00f, 44.70f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-4.90f, 0.00f, -42.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-57.30f, 20.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(82.00f, 0.00f, -4.00f));
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

        InstantiateThrowable(pNewNode, Vector3(5.56f, 1.10f, -16.71f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-26.16f, 22.07f, -7.74f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(38.73f, 1.09f, 24.95f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(56.80f, 1.10f, -9.90f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-3.29f, 1.10f, 13.21f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(27.43f, 0.0f, 23.43f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(-46.85f, 0.64f, -25.32f), 1.0f);
		InstantiateRock3(pNewNode, Vector3(-2.85f, -0.66f, 40.08f), 1.0f, Vector3(0.00f, 0.00f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(-17.35f, -0.26f, -29.62f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRockDouble(pNewNode, Vector3(41.15f, 0.24f, -25.82f), 1.0f, Vector3(0.00f, 1.54f, 0.00f));
        InstantiatePlatform5(pNewNode, Vector3(-35.23f, 1.20f, 3.40f), 1.0f, Vector3(0.00f, 0.18f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(25.70f, 0.50f, 2.20f), 1.0f, Vector3(-0.00f, 3.14f, 0.00f));
        InstantiateColumn2(pNewNode, Vector3(-39.60f, 20.50f, -12.60f), 1.0f, Vector3(0.00f, -1.05f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(-16.20f, 0.10f, -2.50f), 1.0f, Vector3(0.00f, -0.21f, 0.09f));
        InstantiateWall1(pNewNode, Vector3(14.40f, 0.00f, 42.80f), 1.0f, Vector3(0.00f, -1.48f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(50.10f, 2.00f, -0.90f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-32.10f, 26.50f, 1.00f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(3.10f, 2.00f, 0.00f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(13.90f, 2.00f, 26.20f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(49.10f, 2.00f, 21.40f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(23.40f, 2.00f, -14.60f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_duza_p.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(8.60f, 5.00f, 49.30f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-5.90f, 0.00f, -41.70f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-61.00f, 5.00f, 4.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(81.90f, 0.00f, 7.80f));
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

        InstantiateThrowable(pNewNode, Vector3(11.67f, 1.10f, -24.42f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-21.93f, 1.11f, -18.26f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(52.97f, 2.17f, 5.29f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(24.18f, 1.10f, 21.74f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-3.79f, 1.10f, 13.21f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(-14.67f, 0.00f, -9.17f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(25.53f, 0.00f, 0.93f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(-52.15f, 0.64f, -8.42f), 1.0f, Vector3(0.00f, 1.03f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(48.35f, -0.26f, 15.98f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRockDouble(pNewNode, Vector3(48.75f, 0.64f, -24.12f), 1.0f, Vector3(0.00f, -1.64f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-12.85f, 0.64f, 42.28f), 1.0f, Vector3(0.00f, -0.78f, 0.0f));
        InstantiateRock2(pNewNode, Vector3(-33.25f, 0.64f, 39.08f), 1.0f, Vector3(0.00f, -1.64f, 0.0f));
        InstantiatePlatform1(pNewNode, Vector3(-40.73f, -0.10f, 7.40f), 1.0f, Vector3(0.00f, 0.18f, 0.00f));
        InstantiatePlatform1(pNewNode, Vector3(-25.13f, -0.20f, 26.20f), 1.0f, Vector3(0.00f, 1.09f, 0.00f));
        InstantiatePlatform1(pNewNode, Vector3(1.77f, -0.20f, 30.40f), 1.0f, Vector3(0.00f, 1.37f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
		auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
		auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
		auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(45.00f, 2.00f, -0.90f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-16.40f, 2.90f, 5.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(7.60f, 2.00f, -7.50f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(13.00f, 2.00f, 11.20f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(61.20f, 2.00f, 18.00f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(25.80f, 2.00f, -16.20f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspy_duza_o.obj")
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-54.60f, 4.30f, 11.10f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -12.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 0.00f, 64.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.50f, 0.00f, -56.70f));
        ////pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        //pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
        //                        2.0f,
        //                        (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-15.70f, 1.19f, -0.30f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-29.33f, 1.19f, 15.04f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(13.19f, 1.19f, -36.45f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(42.50f, 1.19f, -7.20f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(40.70f, 1.20f, 37.78f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-0.37f, 1.81f, 23.97f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(3.77f, 13.25f, 6.87f), 0.4f);
        InstantiateWall2(pNewNode, Vector3(-41.65f, 0.14f, 11.98f), 1.0f, Vector3(0.00f, 0.12f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(53.85f, -0.26f, 29.08f), 1.0f, Vector3(0.00f, 1.74f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-44.45f, -0.26f, -14.62f), 1.0f, Vector3(0.00f, 0.28f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(-0.95f, -0.26f, 6.38f), 0.8f, Vector3(0.00f, 0.28f, 0.00f));
        InstantiateWall1(pNewNode, Vector3(15.55f, 0.24f, -47.92f), 1.0f, Vector3(0.00f, -1.64f, 0.00f));
        InstantiateRuin5(pNewNode, Vector3(26.75f, 0.64f, 10.18f), 1.0f, Vector3(0.00f, -1.64f, 0.0f));
        InstantiateFire1(pNewNode, Vector3(23.57f, 0.55f, -20.70f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(-16.93f, 0.55f, -21.50f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(24.97f, 0.55f, 34.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateFire1(pNewNode, Vector3(-17.23f, 0.55f, 33.30f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(41.80f, 2.00f, -21.80f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-17.50f, 2.90f, 11.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(4.20f, 2.00f, -19.70f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(5.90f, 2.00f, 34.60f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(36.90f, 2.00f, 26.10f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(23.60f, 2.00f, -4.80f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspy_duza_o.obj")
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-53.70f, 0.00f, -2.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -12.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 0.00f, 61.70f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(12.00f, 3.40f, -55.00f));
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

        InstantiateThrowable(pNewNode, Vector3(-15.70f, 1.19f, -0.30f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-29.33f, 1.19f, 15.04f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(27.08f, 2.26f, -1.62f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(42.50f, 1.19f, -7.20f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(35.80f, 1.20f, 27.18f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(10.60f, 5.84f, 7.45f), 0.4f);
        InstantiateWall1(pNewNode, Vector3(-42.05f, 0.24f, 18.88f), 1.0f, Vector3(0.00f, 0.28f, 0.00f));
        InstantiateWall2(pNewNode, Vector3(10.65f, 0.04f, -47.82f), 1.0f, Vector3(0.00f, -1.64f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(51.65f, -0.26f, 25.68f), 1.0f, Vector3(0.00f, 0.19f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-41.15f, -0.26f, -24.22f), 1.0f, Vector3(0.00f, 0.68f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(50.45f, -0.36f, 12.58f), 1.0f, Vector3(0.00f, 0.31f, 0.00f));
        InstantiateRuin3(pNewNode, Vector3(7.75f, 0.44f, 9.18f), 1.0f, Vector3(0.00f, -1.64f, 0.0f));
        InstantiateStoneStack1(pNewNode, Vector3(1.97f, 2.10f, 31.30f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(31.37f, 1.20f, -33.20f), 1.0f);
        InstantiateMushroom1(pNewNode, Vector3(23.57f, 0.40f, -20.70f), 0.4f, Vector3(0.00f, -2.88f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(-19.73f, 0.40f, -25.00f), 0.4f, Vector3(0.00f, -1.12f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(25.17f, 0.40f, 28.30f), 0.4f, Vector3(0.00f, 2.30f, 0.00f));
        InstantiateMushroom3(pNewNode, Vector3(-12.03f, 0.40f, 21.60f), 0.4f, Vector3(0.00f, 0.65f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(7.70f, 2.00f, -32.80f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-17.50f, 2.90f, 11.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(4.70f, 2.00f, -9.00f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(11.60f, 2.00f, 24.40f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(28.70f, 2.00f, 14.20f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(36.60f, 2.00f, -15.60f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspy_duza_o.obj")
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-52.10f, 0.00f, -2.10f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -19.90f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-7.60f, -3.00f, 62.30f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-15.70f, 0.00f, -56.50f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-27.64f, 1.20f, -22.02f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-6.89f, 1.20f, -38.47f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(8.40f, 1.20f, -41.59f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(46.30f, 1.19f, -12.50f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(26.39f, 1.19f, -33.83f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-41.25f, 1.20f, 5.77f), 0.4f);
		InstantiateRock2(pNewNode, Vector3(1.75f, -1.56f, 35.58f), 1.0f, Vector3(0.00f, 0.00f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(19.90f, 0.74f, 42.90f), 0.7f, Vector3(0.00f, -2.64f, 0.0f));
        InstantiateRockDouble(pNewNode, Vector3(50.20f, 0.34f, -2.20f), 0.7f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRuin3(pNewNode, Vector3(8.40f, 5.64f, 45.10f), 1.0f, Vector3(-0.54f, 1.77f, -3.14f));
		InstantiateRuin3(pNewNode, Vector3(8.80f, 0.44f, 31.10f), 0.9f, Vector3(0.00f, -1.54f, 0.00f));
        InstantiateWall3(pNewNode, Vector3(22.95f, 0.04f, 21.28f), 1.0f, Vector3(0.00f, -0.90f, 0.00f));
        InstantiateStoneStack1(pNewNode, Vector3(-27.33f, 2.10f, 24.20f), 1.0f);
		InstantiateNewColumn(pNewNode, Vector3(-15.03f, 0.30f, -34.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(-15.63f, 0.30f, 30.60f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(36.47f, 0.30f, -28.00f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateColumn2(pNewNode, Vector3(-32.23f, 0.50f, -2.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
		InstantiateBaseColumn(pNewNode, Vector3(7.67f, -2.50f, -2.50f), 12.0f, 1.0f, Vector3(0.00f, 0.0f, 0.00f));
		InstantiatePlatform3(pNewNode, Vector3(38.27f, 0.40f, 29.60f), 0.8f, Vector3(0.00f, 2.25f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-1.80f, 2.00f, -18.60f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-12.20f, 2.90f, -4.40f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(3.10f, 2.00f, 13.80f));
        spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(22.00f, 2.00f, 6.20f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(20.60f, 2.00f, -14.90f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(8.00f, 2.00f, -2.80f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
        pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
        pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspy_duza_o.obj")
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

        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-56.10f, 6.10f, 27.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(67.00f, 0.00f, -12.00f));
        upPoint->SetLocalPosition(DirectX::XMFLOAT3(5.70f, 5.00f, 64.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.40f, 18.00f, -58.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(43.34f, 1.20f, -24.72f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-8.04f, 21.12f, -36.65f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(8.55f, 1.19f, -15.60f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(31.00f, 1.19f, 33.30f), 0.4f);
        InstantiateBrick(pNewNode, Vector3(-0.61f, 9.99f, 27.43f), 0.5f);
        InstantiateBrick(pNewNode, Vector3(43.35f, 0.50f, 3.65f), 0.5f, Vector3(-0.22f, 1.42f, 1.78f));
		InstantiateRock1(pNewNode, Vector3(-11.70f, 0.64f, 1.50f), 0.6f, Vector3(0.00f, -0.86f, 0.00f));
		InstantiateRock2(pNewNode, Vector3(32.00f, 0.64f, -44.40f), 1.0f, Vector3(0.00f, -0.86f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-11.40f, 0.64f, 51.50f), 1.0f, Vector3(0.00f, -0.86f, 0.00f));
		InstantiateRock3(pNewNode, Vector3(-30.90f, 8.04f, 27.70f), 1.0f, Vector3(0.00f, -1.87f, -1.57f));
        InstantiateRock3(pNewNode, Vector3(-16.60f, 5.64f, 15.70f), 0.9f, Vector3(0.00f, 1.38f, -1.57f));
        InstantiateRock4(pNewNode, Vector3(-15.20f, 0.34f, -17.00f), 1.1f, Vector3(-0.00f, -0.53f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(-11.10f, 0.34f, -8.40f), 0.6f, Vector3(0.00f, 0.35f, 0.0f));
		InstantiateRockDouble(pNewNode, Vector3(-29.50f, 0.34f, 43.10f), 0.4f, Vector3(0.00f, -2.29f, 3.14f));
        InstantiateWall4(pNewNode, Vector3(0.97f, -0.60f, 34.20f), 1.0f, Vector3(0.00f, -1.57f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(15.97f, -2.50f, -26.70f), 1.0f, Vector3(0.00f, -1.57f, 0.00f));
        InstantiatePlatform2(pNewNode, Vector3(-5.73f, -0.20f, 26.30f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiatePlatform4(pNewNode, Vector3(-18.23f, -5.40f, -25.00f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));


        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());
        auto spawnPoint5 = std::make_unique<Node>("SpawnPoint 5", pNewNodeOwner.get());
        auto spawnPoint6 = std::make_unique<Node>("SpawnPoint 6", pNewNodeOwner.get());
        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint5->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint5.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint5->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint6->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint6.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint6->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(20.60f, 2.00f, -15.70f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(39.20f, 2.90f, 20.60f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(-1.10f, 10.20f, 29.80f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(40.60f, 2.00f, -8.20f));
		spawnPoint5->SetLocalPosition(DirectX::XMFLOAT3(-10.10f, 2.00f, -16.10f));
		spawnPoint6->SetLocalPosition(DirectX::XMFLOAT3(25.20f, 2.00f, 34.40f));
        pIsland->spawnPoints.push_back(spawnPoint1.get());
        pIsland->spawnPoints.push_back(spawnPoint2.get());
        pIsland->spawnPoints.push_back(spawnPoint3.get());
		pIsland->spawnPoints.push_back(spawnPoint4.get());
		pIsland->spawnPoints.push_back(spawnPoint5.get());
		pIsland->spawnPoints.push_back(spawnPoint6.get());
        pNewNode->AddChild(std::move(spawnPoint1));
        pNewNode->AddChild(std::move(spawnPoint2));
        pNewNode->AddChild(std::move(spawnPoint3));
		pNewNode->AddChild(std::move(spawnPoint4));
		pNewNode->AddChild(std::move(spawnPoint5));
		pNewNode->AddChild(std::move(spawnPoint6));

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
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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
        /*pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
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
        rightPoint->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
        pIsland->leftPoint = leftPoint.get();
        pIsland->rightPoint = rightPoint.get();
        pIsland->upPoint = upPoint.get();
        pIsland->downPoint = downPoint.get();
        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();

        InstantiateThrowable(pNewNode, Vector3(9.28f, 0.65f, 15.50f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(24.69f, 1.71f, 22.19f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(20.44f, 1.30f, -14.47f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-10.99f, 1.30f, -6.94f), 0.4f);
		InstantiateStoneStack1(pNewNode, Vector3(25.00f, 1.10f, 3.30f), 1.0f);
        InstantiateStoneStack1(pNewNode, Vector3(-7.20f, 1.10f, -14.00f), 1.0f, Vector3(0.00f, 1.71f,  0.00f));
        InstantiateStoneStack1(pNewNode, Vector3(-4.90f, 1.10f, 20.40f), 1.0f, Vector3(0.00f, -0.40f, 0.00f));
		InstantiateRock1(pNewNode, Vector3(-26.70f, 0.04f, -10.60f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-30.50f, 0.04f, 14.50f), 1.0f, Vector3(0.00f, 2.44f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(32.00f, 0.04f, -23.10f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-33.70f, 0.04f, 3.50f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(4.40f, 2.00f, 1.40f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-13.30f, 2.90f, 6.60f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(18.60f, 2.00f, 13.10f));
        spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(11.20f, 2.00f, -13.30f));
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
    static Node* InstantiateIslandMedium2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M2", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(2.70f, 0.00f, 39.90f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-19.40f, 0.00f, -33.30f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-40.60f, 0.00f, 4.70f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.10f, 29.10f, -6.60f));
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

        InstantiateThrowable(pNewNode, Vector3(-6.52f, 0.65f, 25.10f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(18.21f, 0.65f, 16.73f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(5.84f, 0.66f, -0.67f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-13.68f, 0.64f, -17.33f), 0.4f);
        InstantiateFire1(pNewNode, Vector3(0.60f, 0.00f, 13.00f), 1.0f);
        InstantiateRuin3(pNewNode, Vector3(29.60f, 7.00f, 16.60f), 1.3f, Vector3(0.58f, -0.00f, -3.14f));
        InstantiateRuinRoof1(pNewNode, Vector3(24.00f, -3.00f, -11.80f), 1.0f);
        InstantiateRock1(pNewNode, Vector3(28.60f, -0.06f, 14.90f), 0.6f, Vector3(0.00f, 1.03f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(-29.20f, 0.04f, -12.40f), 1.0f, Vector3(0.00f, 1.03f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-31.50f, 0.04f, 16.30f), 1.0f, Vector3(0.00f, 2.44f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(28.60f, 0.04f, 5.00f), 1.5f, Vector3(0.00f, -2.72f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(-28.60f, -0.06f, 23.60f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(11.50f, 2.00f, -10.50f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-17.60f, 2.90f, 4.30f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(10.80f, 2.00f, 24.40f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(24.50f, 25.50f, -8.90f));
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
    static Node* InstantiateIslandMedium3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M3", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(25.30f, 20.90f, 39.90f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(6.80f, 3.60f, -34.80f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-40.60f, 13.00f, -3.00f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.40f, 0.00f, -12.20f));
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

        InstantiateThrowable(pNewNode, Vector3(-16.38f, 0.66f, 25.11f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(25.13f, 0.65f, 3.41f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(27.87f, 0.66f, -18.04f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-3.28f, 0.64f, -15.43f), 0.4f);
        InstantiateFire1(pNewNode, Vector3(3.20f, 0.00f, 3.60f), 1.0f);
        InstantiateRuin3(pNewNode, Vector3(4.60f, 6.10f, 22.80f), 1.0f, Vector3(0.53f, -1.57f, -3.14f));
        InstantiateRuinRoof2(pNewNode, Vector3(22.80f, -0.10f, 21.30f), 1.0f, Vector3(0.00f, -1.48f, 0.00f));
        InstantiateWall3(pNewNode, Vector3(-30.70f, -0.36f, 1.10f), 1.0f, Vector3(0.00f, 3.00f, 0.00f));
        InstantiateWall2(pNewNode, Vector3(7.70f, -0.26f, -27.00f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(14.10f, 0.04f, 21.80f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(-24.50f, 0.04f, -3.30f), 1.5f, Vector3(0.00f, -2.90f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(3.30f, -1.06f, 24.50f), 0.6f, Vector3(0.00f, -2.90f, 0.00f));
		InstantiateRock3(pNewNode, Vector3(-24.00f, 0.04f, -23.60f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

       /* spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(12.00f, 2.00f, -17.60f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(27.00f, 2.90f, 12.90f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(-6.90f, 2.00f, 9.30f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(25.60f, 2.00f, -10.20f));
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
    static Node* InstantiateIslandMedium4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M4", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(18.80f, 0.00f, 38.70f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(3.30f, 0.00f, -35.40f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-40.60f, 0.00f, -6.30f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.40f, 4.30f, -12.20f));
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

        InstantiateThrowable(pNewNode, Vector3(11.46f, 0.64f, 1.84f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(32.25f, 7.80f, -7.99f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(8.98f, 1.22f, -23.32f), 0.4f);
        InstantiateBrick(pNewNode, Vector3(17.08f, 3.13f, 18.48f), 0.5f, Vector3(-0.22f, 1.32f, 1.79f));
        InstantiateBrick(pNewNode, Vector3(-25.09f, -0.02f, -4.26f), 0.5f, Vector3(-0.00f, 0.61f, 0.03f));
        InstantiateBrick(pNewNode, Vector3(-3.17f, 6.05f, 16.21f), 0.5f, Vector3(-0.21f, 2.11f, 1.76f));
        InstantiateMushroom1(pNewNode, Vector3(-11.40f, 0.00f, -21.70f), 0.35f, Vector3(0.00f, -0.63f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(9.80f, 0.00f, 13.00f), 0.4f, Vector3(0.00f, 1.01f, 0.00f));
        InstantiatePlatform1(pNewNode, Vector3(26.40f, -0.60f, -13.40f), 1.0f, Vector3(0.00f, -3.14f, 0.00f));
		InstantiateRuin2(pNewNode, Vector3(-15.70f, -1.16f, 22.00f), 1.0f, Vector3(-0.00f, -1.90f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-30.30f, 0.04f, 10.90f), 1.0f, Vector3(0.00f, -0.14f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(-20.70f, 0.04f, -22.80f), 0.6f, Vector3(0.00f, 2.69f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(30.80f, -0.16f, 5.90f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-14.60f, 5.60f, 16.90f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(0.60f, 2.90f, -14.00f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(29.20f, 2.00f, 19.40f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(0.50f, 3.40f, -0.20f));
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
    static Node* InstantiateIslandMedium5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M5", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(18.20f, 4.80f, 38.70f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(20.70f, -1.60f, -36.70f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-41.00f, 0.00f, 13.40f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.40f, 4.30f, -15.20f));
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

        InstantiateThrowable(pNewNode, Vector3(-24.05f, 1.30f, 18.10f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-21.22f, 1.34f, -17.46f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(19.46f, 8.46f, 21.57f), 0.4f);
        InstantiateBrick(pNewNode, Vector3(-8.44f, 2.27f, 13.87f), 0.5f, Vector3(-0.22f, 1.32f, 1.79f));
        InstantiateBrick(pNewNode, Vector3(6.81f, -0.00f, -20.35f), 0.5f, Vector3(-0.00f, 0.61f, 0.03f));
        InstantiateWall1(pNewNode, Vector3(-11.37f, -0.20f, 30.41f), 1.0f, Vector3(0.0f, 1.40f, 0.0f));
        InstantiateMushroom1(pNewNode, Vector3(-18.10f, 0.00f, -4.70f), 0.35f, Vector3(0.00f, 0.98f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(8.50f, 5.30f, 17.00f), 0.4f, Vector3(0.00f, -0.70f, 0.00f));
        InstantiatePlatform2(pNewNode, Vector3(14.60f, -0.60f, 17.00f), 0.7f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiateColumn2(pNewNode, Vector3(20.70f, -0.26f, -25.70f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(35.10f, 0.04f, 19.50f), 1.0f, Vector3(0.00f, 0.40f, 0.00f));
        InstantiateRockDouble(pNewNode, Vector3(-31.20f, -0.16f, -8.90f), 0.6f, Vector3(-0.00f, 3.02f, 0.0f));
        InstantiateRock3(pNewNode, Vector3(34.20f, 0.94f, 14.60f), 1.0f, Vector3(1.56f, -3.14f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

       /* spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/


        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(19.70f, 5.60f, 5.00f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-13.40f, 2.90f, 2.40f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(21.90f, 2.00f, -16.60f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(-7.00f, 3.40f, -19.50f));
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
    static Node* InstantiateIslandMedium6(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M6", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(7.70f, 4.80f, 41.10f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(-2.80f, 3.20f, -36.70f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-41.50f, 0.00f, -6.60f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.40f, 4.30f, 14.50f));
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

        InstantiateThrowable(pNewNode, Vector3(-2.95f, 0.64f, 24.60f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-9.93f, 0.66f, 4.53f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(16.38f, -0.43f, -3.74f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(21.56f, 8.46f, 20.77f), 1.0f);
        InstantiateBrick(pNewNode, Vector3(-7.75f, 2.11f, -20.27f), 0.5f, Vector3(-0.22f, 1.38f, 1.77f));
        InstantiateBrick(pNewNode, Vector3(27.39f, 0.01f, 5.09f), 0.5f, Vector3(-0.00f, 0.61f, 0.03f));
        InstantiateSpike1(pNewNode, Vector3(6.30f, -3.70f, -15.20f), 1.0f, Vector3(0.00f, -3.14f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(20.50f, -3.90f, -26.80f), 1.0f, Vector3(0.00f, -1.57f, 0.00f));
        InstantiateWall4(pNewNode, Vector3(19.00f, -0.30f, -18.20f), 0.7f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiateColumn3(pNewNode, Vector3(9.53f, -0.20f, 11.61f), 1.0f, Vector3(0.0f, -1.57f, 0.0f));
        InstantiateColumn4(pNewNode, Vector3(-12.90f, -0.26f, -4.90f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiateRock1(pNewNode, Vector3(-22.20f, -1.16f, 17.10f), 1.3f, Vector3(0.00f, 0.40f, 0.00f));
		InstantiateRock1(pNewNode, Vector3(34.70f, -1.56f, -1.50f), 0.6f, Vector3(0.00f, -1.24f, 0.00f));
        InstantiateRock4(pNewNode, Vector3(-24.80f, 3.74f, 13.40f), 1.0f, Vector3(1.56f, -2.65f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(21.40f, 3.00f, -11.90f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(0.60f, 2.90f, 5.10f));
        spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(-21.40f, 2.00f, -8.20f));
        spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(12.00f, 3.40f, 20.10f));
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
    static Node* InstantiateIslandMedium7(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M7", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-4.80f, 0.00f, 40.50f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(1.70f, 4.10f, -36.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-42.00f, 0.00f, 6.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.40f, 0.00f, 11.10f));
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

        InstantiateThrowable(pNewNode, Vector3(-3.69f, 0.66f, 27.33f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-27.87f, 0.66f, -5.30f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(29.86f, 0.64f, 1.76f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(11.36f, 8.46f, 19.27f), 1.0f);
        InstantiateFire1(pNewNode, Vector3(-19.00f, 0.00f, 20.30f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
		InstantiateWall1(pNewNode, Vector3(24.00f, -1.30f, -26.70f), 1.0f, Vector3(0.00f, 1.50f, 0.00f));
        InstantiateWall1(pNewNode, Vector3(-27.60f, -1.00f, -18.00f), 1.0f, Vector3(0.00f, -0.93f, -0.00f));
        InstantiateRuin2(pNewNode, Vector3(1.50f, -1.30f, -17.50f), 1.0f, Vector3(0.00f, 1.57f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(19.23f, -0.40f, 2.71f), 1.0f, Vector3(0.0f, 0.0f, 0.0f));
        InstantiateColumn4(pNewNode, Vector3(-17.30f, -0.46f, 3.70f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(31.80f, -1.16f, -20.50f), 1.3f, Vector3(0.00f, 1.01f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(35.60f, -0.56f, -8.30f), 1.0f, Vector3(0.00f, 0.40f, 0.00f));
        InstantiateRock3(pNewNode, Vector3(33.00f, -0.16f, 26.10f), 1.0f, Vector3(0.0f, -2.65f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(23.90f, 3.00f, -8.20f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-19.10f, 2.90f, -11.20f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(0.70f, 5.20f, -9.70f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(21.50f, 3.40f, 17.70f));
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
    static Node* InstantiateIslandMedium8(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M8", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-4.80f, 12.40f, 40.50f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(1.70f, 0.00f, -36.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-42.00f, 13.80f, 6.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.50f, 0.00f, -6.60f));
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

        InstantiateThrowable(pNewNode, Vector3(-29.60f, 21.27f, 10.45f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-12.59f, 0.64f, -17.19f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(27.96f, 0.64f, -1.34f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(0.16f, 8.46f, -8.43f), 1.0f, Vector3(0.0f, 0.90f, 0.0f));
        InstantiateStoneStack1(pNewNode, Vector3(16.20f, -0.30f, -21.60f), 1.0f, Vector3(0.00f, 1.50f, 0.00f));
        InstantiatePlatform4(pNewNode, Vector3(-18.00f, -2.70f, 16.70f), 0.7f, Vector3(0.00f, 1.74f, 0.00f));
        InstantiateSpike1(pNewNode, Vector3(-16.67f, -2.80f, 0.81f), 1.0f, Vector3(0.00f, 0.86f, 0.22f));
        InstantiateFire1(pNewNode, Vector3(8.40f, 0.04f, -13.60f), 1.0f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateRock4(pNewNode, Vector3(20.80f, -1.16f, -28.40f), 0.6f, Vector3(0.00f, 1.01f, 0.00f));
        InstantiateRock4(pNewNode, Vector3(30.70f, -0.56f, -23.30f), 1.0f, Vector3(0.00f, 0.40f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(13.80f, -0.56f, 28.80f), 1.0f, Vector3(0.0f, -3.07f, 0.0f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(25.00f, 3.00f, 8.80f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-12.20f, 2.90f, -11.20f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 2.00f, 4.80f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(17.60f, 3.40f, -6.70f));
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
    static Node* InstantiateIslandMedium9(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island M9", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_srednia.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(9.40f, 2.90f, 40.50f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(1.70f, 0.00f, -36.00f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-42.00f, 0.00f, 6.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(45.50f, 0.00f, -6.60f));
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

        InstantiateThrowable(pNewNode, Vector3(-27.60f, 0.65f, 9.25f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-5.49f, 0.64f, -20.79f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(29.06f, 0.64f, -12.94f), 0.4f);
        InstantiateMushroom1(pNewNode, Vector3(-13.10f, -0.30f, -7.20f), 0.4f, Vector3(0.00f, 0.99f, 0.00f));
        InstantiateMushroom2(pNewNode, Vector3(11.20f, -0.30f, -4.70f), 0.35f, Vector3(-0.00f, 2.02f, 0.00f));
        InstantiateMushroom3(pNewNode, Vector3(-16.40f, -0.30f, -7.10f), 0.2f, Vector3(0.00f, 0.0f, 0.00f));
        InstantiateMushroom3(pNewNode, Vector3(14.40f, -0.30f, -5.00f), 0.2f, Vector3(0.00f, 0.0f, 0.00f));
		InstantiateNewColumn(pNewNode, Vector3(11.36f, -0.30f, 1.57f), 1.0f, Vector3(0.0f, 0.0f, 0.0f));
        InstantiateNewColumn(pNewNode, Vector3(-10.84f, -0.30f, 4.87f), 1.0f, Vector3(0.0f, 0.0f, 0.0f));
        InstantiateColumn3(pNewNode, Vector3(11.16f, -0.30f, -19.33f), 1.0f, Vector3(0.0f, 1.57f, 0.0f));
		InstantiateWall2(pNewNode, Vector3(8.50f, -0.30f, 32.00f), 1.0f, Vector3(0.00f, 1.66f, 0.00f));
		InstantiateRuin3(pNewNode, Vector3(-13.40f, -0.20f, 23.10f), 1.5f, Vector3(0.00f, 1.68f, 0.00f));
        InstantiateRock4(pNewNode, Vector3(-30.80f, -1.16f, -14.50f), 1.2f, Vector3(0.00f, 1.01f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
        auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 3", pNewNodeOwner.get());
        auto spawnPoint4 = std::make_unique<Node>("SpawnPoint 4", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint3->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint3.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint3->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint4->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint4.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint4->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(11.00f, 3.00f, 13.30f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-1.60f, 2.90f, -11.20f));
		spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(-18.90f, 2.00f, 5.80f));
		spawnPoint4->SetLocalPosition(DirectX::XMFLOAT3(23.50f, 3.40f, -6.70f));
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


    static Node* InstantiateIslandSmall1(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(-10.00f, 4.10f, 33.00f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -26.80f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-30.50f, 2.10f, 5.00f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(34.00f, 0.00f, -10.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateBrick(pNewNode, Vector3(-13.98f, 0.69f, -11.85f), 0.5f);
        InstantiateThrowable(pNewNode, Vector3(15.12f, 1.45f, -17.33f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-9.2f, 3.0f, 16.2f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(19.2f, 3.0f, -5.2f), 0.4f);
        InstantiatePlatform2(pNewNode, Vector3(-7.20f, -0.40f, 11.50f), 0.75f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(10.00f, 2.00f, -5.10f));
		spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-10.00f, 7.80f, 3.10f));
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
    static Node* InstantiateIslandSmall2(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {


        auto pNewNodeOwner = std::make_unique<Node>("Island S2", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala_L.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(18.70f, 0.00f, 31.30f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -25.0f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-30.50f, 0.00f, -4.70f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -8.50f));
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

        InstantiateThrowable(pNewNode, Vector3(0.58f, 0.24f, -14.49f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(18.2f, 3.0f, 3.2f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(15.29f, 1.04f, 17.81f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-13.2f, 3.0f, -2.2f), 0.4f);
        InstantiateNewColumn(pNewNode, Vector3(12.0f, -0.70f, -5.0f), 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
		auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(17.20f, 2.00f, -14.00f));
		spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-10.00f, 2.00f, -8.10f));
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
    static Node* InstantiateIslandSmall3(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0}) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S3", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -26.60f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.40f, 0.00f, 8.00f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.50f, 0.00f, 1.00f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-9.0f, 15.0f, -9.0f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(8.2f, 15.0f, -10.2f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(-9.2f, 15.0f, 9.2f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(8.2f, 15.0f, 9.2f), 0.4f);
        InstantiateNewColumn(pNewNode, Vector3(-14.50f, 0.00f, 18.50f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(14.50f, 0.00f, -12.00f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(14.50f, 0.00f, 18.50f), 1.0f);
        InstantiateColumn4(pNewNode, Vector3(-14.50f, 0.00f, -12.00f), 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
		auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 2.00f, -7.50f));
		spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 2.00f, 15.20f));
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
    static Node* InstantiateIslandSmall4(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S4", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.60f, 0.00f, 7.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-12.92f, 1.97f, 1.39f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(16.37f, 0.90f, -13.52f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(13.66f, 0.91f, 17.48f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(8.70f, 15.00f, -7.60f), 1.0f);
        InstantiateRock1(pNewNode, Vector3(-12.70f, 0.00f, 18.30f), 1.0f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
		auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

       /* spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(3.90f, 2.00f, 11.50f));
		spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-4.30f, 2.00f, -9.10f));
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
    static Node* InstantiateIslandSmall5(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S5", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.60f, 0.00f, 7.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(1.02f, 7.23f, 1.34f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(5.53f, 0.90f, -16.31f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(4.76f, 0.91f, 21.68f), 0.4f);
		InstantiateFire1(pNewNode, Vector3(-13.80f, 0.30f, 19.90f), 1.0f);
        InstantiateNewColumn(pNewNode, Vector3(16.70f, -0.10f, -13.30f), 1.0f);
        InstantiatePlatform3(pNewNode, Vector3(-0.30f, -0.10f, 0.80f), 0.4f);

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(16.70f, 2.00f, 16.90f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-8.90f, 2.00f, -12.70f));
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
    static Node* InstantiateIslandSmall6(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S6", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.60f, 0.00f, 7.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-12.09f, 0.90f, -2.74f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(11.53f, 0.90f, -15.91f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(0.16f, 0.91f, 21.68f), 0.4f);
        InstantiateMushroom1(pNewNode, Vector3(16.50f, 0.20f, -8.40f), 0.35f, Vector3(0.00f, -2.58f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(-9.50f, -0.10f, 12.40f), 1.0f);
        InstantiateWall1(pNewNode, Vector3(-20.20f, -0.10f, -4.20f), 1.0f, Vector3(0.00f, -0.24f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(11.30f, 2.00f, 9.70f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-2.90f, 2.00f, -8.30f));
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
    static Node* InstantiateIslandSmall7(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S7", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.60f, 0.00f, 7.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-12.09f, 0.90f, -2.74f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(19.27f, 0.90f, -1.42f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(0.16f, 0.91f, 21.68f), 0.4f);
        InstantiateBrick(pNewNode, Vector3(-0.70f, 0.20f, -10.76f), 0.5f, Vector3(0.00f, -2.58f, 0.00f));
        InstantiateNewColumn(pNewNode, Vector3(-9.10f, -0.10f, 13.60f), 1.0f);
        InstantiateColumn3(pNewNode, Vector3(12.00f, -0.10f, -9.80f), 1.0f, Vector3(0.00f, 0.00f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(11.80f, 2.00f, 10.70f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-2.90f, 2.00f, -0.70f));
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
    static Node* InstantiateIslandSmall8(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S8", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.60f, 0.00f, 7.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-16.19f, 0.90f, 6.96f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(19.59f, 0.90f, -13.37f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(13.22f, 2.21f, 16.74f), 0.4f);
        InstantiatePot3(pNewNode, Vector3(3.78f, 0.66f, 4.93f), 0.5f);
        InstantiateFire1(pNewNode, Vector3(-5.80f, 0.30f, -8.76f), 1.0f, Vector3(0.00f, 0.00f, 0.00f));
        InstantiatePlatform2(pNewNode, Vector3(10.60f, -2.60f, 14.20f), 0.5f, Vector3(0.00f, 0.00f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(13.90f, 2.00f, -3.40f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-3.10f, 2.00f, 13.30f));
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
    static Node* InstantiateIslandSmall9(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S9", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.00f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.60f, 0.00f, 7.50f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-14.69f, 0.90f, -3.04f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(15.59f, 0.90f, 5.53f), 0.4f);
        InstantiatePot3(pNewNode, Vector3(10.43f, 2.00f, -15.37f), 0.5f);
        InstantiatePot4(pNewNode, Vector3(-2.05f, 0.66f, 12.33f), 0.5f);
        InstantiateMushroom2(pNewNode, Vector3(-7.80f, 0.20f, 9.34f), 0.35f, Vector3(0.00f, 0.72f, 0.00f));
        InstantiateRock2(pNewNode, Vector3(-14.80f, 0.00f, -17.40f), 1.0f, Vector3(0.00f, 0.77f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(18.50f, -0.60f, 20.10f), 1.0f, Vector3(0.00f, -0.45f, 0.00f));
		InstantiateColumn4(pNewNode, Vector3(15.40f, -0.10f, -10.50f), 1.0f, Vector3(0.00f, 0.00f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(10.60f, 2.00f, -3.40f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-13.60f, 2.00f, 7.90f));
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
    static Node* InstantiateIslandSmall10(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S10", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala_L.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(16.20f, 0.00f, 32.60f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(7.30f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-30.20f, 0.00f, 2.90f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.00f, 0.00f, -4.40f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-11.79f, 0.22f, 3.06f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(19.87f, 0.22f, -2.51f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(10.06f, 0.84f, -13.87f), 0.4f);
        InstantiateMushroom2(pNewNode, Vector3(-5.50f, -0.50f, -11.26f), 0.35f, Vector3(0.00f, -0.52f, 0.00f));
        InstantiateMushroom3(pNewNode, Vector3(19.50f, -0.50f, 2.64f), 0.35f, Vector3(0.00f, 0.72f, 0.00f));
        InstantiateRock4(pNewNode, Vector3(-15.40f, -0.60f, -14.60f), 1.0f, Vector3(0.00f, -1.43f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(7.20f, 2.00f, -3.40f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(17.10f, 2.00f, 14.60f));
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
    static Node* InstantiateIslandSmall11(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S11", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala_L.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(13.70f, 0.00f, 31.10f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-29.30f, 0.00f, -13.80f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.20f, 0.00f, -14.10f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
        /*pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                                2.0f,
                                (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };*/
       /* upPoint->AddComponent(
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

        InstantiateThrowable(pNewNode, Vector3(-4.68f, 0.23f, 3.05f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(18.47f, 0.22f, 21.19f), 0.4f);
        InstantiateBrick(pNewNode, Vector3(-0.79f, 2.84f, -16.19f), 0.5f);
        InstantiateFire1(pNewNode, Vector3(6.10f, -0.40f, -2.06f), 1.0f, Vector3(0.00f, -0.52f, 0.00f));
        InstantiateColumn2(pNewNode, Vector3(15.20f, -0.50f, -9.96f), 1.0f, Vector3(0.00f, 0.72f, 0.00f));
        InstantiateRock5(pNewNode, Vector3(-17.50f, -1.00f, 0.50f), 1.0f, Vector3(0.00f, -1.43f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-8.80f, 2.00f, -13.70f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(11.40f, 2.00f, 8.90f));
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
    static Node* InstantiateIslandSmall12(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        auto pNewNodeOwner = std::make_unique<Node>("Island S12", nullptr, "GROUND");

        pNewNodeOwner->AddComponent(
            std::make_unique<ModelComponent>(pNewNodeOwner.get(), wind->Gfx(), "Models\\wyspy\\wyspa_mala_L.obj")
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

        upPoint->SetLocalPosition(DirectX::XMFLOAT3(13.70f, 0.00f, 31.10f));
        downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.00f, 0.00f, -27.50f));
        leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-27.70f, 0.00f, -0.40f));
        rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.20f, 0.00f, -14.10f));
        //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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

        InstantiateThrowable(pNewNode, Vector3(-6.38f, 0.23f, 0.65f), 0.4f);
        InstantiateThrowable(pNewNode, Vector3(17.17f, 0.22f, 13.89f), 0.4f);
        InstantiateStoneStack1(pNewNode, Vector3(2.41f, 2.84f, -13.59f), 1.0f);
        InstantiateRock3(pNewNode, Vector3(20.70f, -0.50f, -0.86f), 1.0f, Vector3(0.00f, 0.51f, 0.00f));
        InstantiatePlatform3(pNewNode, Vector3(-17.70f, -2.20f, -1.40f), 0.3f, Vector3(0.00f, -0.23f, 0.00f));

        auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
        auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());

        /*spawnPoint1->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint1.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint1->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        spawnPoint2->AddComponent(
            std::make_unique<ModelComponent>(spawnPoint2.get(), wind->Gfx(), "Models\\kolumna\\kolumna.obj")
        );
        spawnPoint2->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/

        spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-7.40f, 2.00f, -15.00f));
        spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(10.40f, 2.00f, -4.20f));
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
    static Node* InstantiateBaseColumn(Node* parentNode, Vector3 position, float scaleRadius, float scaleHight, Vector3 rotation = {0,0,0})
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
        ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scaleRadius, scaleHight, scaleRadius));
        islandMeshShape = islandScaling.Create().Get();
        BodyCreationSettings bodySettings(islandMeshShape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
        bodySettings.mFriction = 1.0f;
        pNewNodeOwner->AddComponent(
            std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
        );

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scaleRadius, scaleHight, scaleRadius));
        pNewNodeOwner->SetLocalRotation(rotation);

        Node* pNewNode = pNewNodeOwner.get();
        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateNewColumn(Node* parentNode, Vector3 position, float scale, Vector3 rotation = {0,0,0})
    {
        auto pNewNodeOwner = std::make_unique<Node>("New Column", nullptr, "WALL");
        Node* pNewNode = pNewNodeOwner.get();

        InstantiateBaseColumn(pNewNode, Vector3(0.0f, 2.0f, 0.0f), 1.0f, 1.0f);
        Node* middle = InstantiateMiddleColumn(pNewNode, Vector3(0.0f, 12.6f, 0.0f), 1.0f);
        Node* top = InstantiateTopColumn(middle, Vector3(0.0f, 7.9f, 0.0f), 1.0f);
        top->GetComponent<Rigidbody>()->ConnectWithOtherBody(middle->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 7.9f, 0.0f));

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateAbility2Particles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability2Particles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 5.0f;                
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ; 
        pCircleLogic->ParticlesPerSecond = 150.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\long.png", 2000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.3f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 1.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 70.0f, 0.0f };
        particles->StartSize = 3.0f;
        particles->bAnimateSize = false;
        particles->StartSizeVariance = 8.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = {1.0f, 1.0f, 1.0f, 0.8f};
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        
        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility2ParticlesSmoke(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability2SmokeParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 5.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 70.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.3f;
        particles->EmissionDuration = 0.22f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 1.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 70.0f, 0.0f };
        particles->StartSize = 3.0f;
        particles->bAnimateSize = false;
        particles->StartSizeVariance = 3.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 0.3f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility1ParticlesSmoke(Node* parentNode, Vector3 position, float scale, DirectX::XMFLOAT4 rotation)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability1SmokeParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 5.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XY;
        pCircleLogic->ParticlesPerSecond = 70.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.3f;
        particles->EmissionDuration = 0.22f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 0.0f, 1.0f };
        particles->ParticleVelocityVariance = { 0.0f, 0.0f, 70.0f };
        particles->StartSize = 3.0f;
        particles->bAnimateSize = false;
        particles->StartSizeVariance = 3.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 0.3f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility3CoreParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability3CoreParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\rzut.png", 200, std::make_unique<PointEmitterLogic>())
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.7f;
        particles->EmissionDuration = 1.0f;
        particles->EmissionRate = 200.0f;
        particles->ParticleVelocity = { 0.0f, 0.0f, 0.0f };
        particles->ParticleVelocityVariance = { 4.0f, 4.0f, 4.0f };
        particles->StartSize = 5.0f;
        particles->EndSize = 5.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility3CoreSmokeParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability3CoreSmokeParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 2000, std::make_unique<PointEmitterLogic>())
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.3f;
        particles->EmissionDuration = 1.0f;
        particles->EmissionRate = 200.0f;
        particles->ParticleVelocity = { 10.0f, 10.0f, 10.0f };
        particles->ParticleVelocityVariance = { -20.0f, -20.0f, -20.0f };
        particles->StartSize = 3.0f;
        particles->bAnimateSize = false;
        particles->StartSizeVariance = 3.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility4SelectParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability4SelectParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 5.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\long.png", 2000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.7f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 5.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 3.0f, 0.0f };
        particles->bAnimateSize = false;
        particles->StartSize = 2.0f;
        particles->StartSizeVariance = 4.0f;
        //particles->EndSize = 1.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility4SelectSmokeParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability4SelectSmokeParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 3.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 2000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.7f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 5.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 3.0f, 0.0f };
        particles->bAnimateSize = false;
        particles->StartSize = 4.0f;
        particles->StartSizeVariance = 3.0f;
        //particles->EndSize = 1.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility6HoldParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability6HoldParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 10.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\long.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.3f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 5.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 3.0f, 0.0f };
        particles->EmitterPositionOffset = {0.0f, -1.5f, 0.0f};
        particles->StartSize = 2.0f;
        particles->StartSizeVariance = 4.0f;
        particles->bAnimateSize = false;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility6HoldSmokeParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability6HoldSmokeParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 3.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.3f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 5.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 3.0f, 0.0f };
        particles->EmitterPositionOffset = { 0.0f, -1.5f, 0.0f };
        particles->StartSize = 4.0f;
        particles->StartSizeVariance = 4.0f;
        particles->bAnimateSize = false;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility5Particles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }, float duration = 1.0f)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability5Particles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 5.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\long.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.9f;
        particles->EmissionDuration = duration;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 10.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 5.0f, 0.0f };
        particles->StartSize = 3.0f;
        particles->StartSizeVariance = 7.0f;
        particles->bAnimateSize = false;
        //particles->EndSize = 1.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility5SmokeParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }, float duration = 1.0f)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability5Particles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 3.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.9f;
        particles->EmissionDuration = duration;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 10.0f, 0.0f };
        particles->ParticleVelocityVariance = { 0.0f, 5.0f, 0.0f };
        particles->StartSize = 4.0f;
        particles->StartSizeVariance = 4.0f;
        particles->bAnimateSize = false;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility4ReleaseParticles(Node* parentNode, Vector3 position, float scale, DirectX::XMFLOAT4 rotation)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability4ReleaseParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XY;
        pCircleLogic->ParticlesPerSecond = 350.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\rzut.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.2f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 0.0f, 1.0f };
        particles->ParticleVelocityVariance = { 0.0f, 0.0f, 70.0f };
        particles->StartSize = 2.0f;
        particles->EndSize = 1.0f;
        particles->StartRotation = 1.57079f;
        particles->EndRotation = 1.57079f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility4ReleaseSmokeParticles(Node* parentNode, Vector3 position, float scale, DirectX::XMFLOAT4 rotation)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability4ReleaseSmokeParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XY;
        pCircleLogic->ParticlesPerSecond = 10.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 2000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.2f;
        particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 0.0f, 1.0f };
        particles->ParticleVelocityVariance = { 0.0f, 0.0f, 70.0f };
        particles->bAnimateSize = false;
        particles->StartSize = 4.0f;
        particles->StartSizeVariance = 4.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility6PullingParticles(Node* parentNode, Vector3 position, float scale, DirectX::XMFLOAT4 rotation)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability6PullingParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 2.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XY;
        pCircleLogic->ParticlesPerSecond = 50.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 2000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.2f;
        //particles->EmissionDuration = 0.1f;
        particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 0.0f, 35.0f };
        particles->ParticleVelocityVariance = { 0.0f, 0.0f, 33.0f };
        particles->StartSize = 4.0f;
        particles->StartSizeVariance = 4.0f;
        particles->bAnimateSize = false;
        particles->StartRotation = 0.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = false;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetWorldPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility3Particles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability3Particles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto sphereEmitter = std::make_unique<SphereToCenterEmitterLogic>();

        // Make the implosion very chaotic
        sphereEmitter->SpeedRandomness = 0.5f;
        sphereEmitter->SpawnRadius = 20.0f;
        sphereEmitter->TravelSpeed = 30.0f;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 2000, std::move(sphereEmitter))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.5f;
        particles->EmissionDuration = 1.0f;
        particles->EmissionRate = 40.0f;
        //particles->ParticleVelocity = { 0.0f, 20.0f, 0.0f };
        //particles->ParticleVelocityVariance = { 0.0f, 10.0f, 0.0f };
        particles->StartSize = 4.0f;
        //particles->EndSize = 1.0f;
        particles->bAnimateSize = false;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateRedPillParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("RedPillParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto sphereEmitter = std::make_unique<SphereToCenterEmitterLogic>();

        // Make the implosion very chaotic
        sphereEmitter->SpeedRandomness = 0.5f;
        sphereEmitter->SpawnRadius = 1.0f;
        sphereEmitter->TravelSpeed = 0.5f;
        sphereEmitter->ParticlesPerSecond = 30;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\flame.png", 20000, std::move(sphereEmitter))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 1.2f;
        particles->EmissionDuration = 1.0f;
        particles->EmissionRate = 4.0f;
        //particles->ParticleVelocity = { 0.0f, 20.0f, 0.0f };
        //particles->ParticleVelocityVariance = { 0.0f, 10.0f, 0.0f };
        particles->StartSize = 0.3f;
        //particles->EndSize = 1.0f;
        particles->bAnimateSize = false;
        particles->EndRotation = 0.0f;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateBluePillParticles(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 })
    {
        auto pNewNodeOwner = std::make_unique<Node>("BluePillParticles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto volumeEmitter = std::make_unique<SphereVolumeEmitterLogic>();

        volumeEmitter->SpawnRadius = 1.0f;
        volumeEmitter->ParticlesPerSecond = 20;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\flame.png", 20000, std::move(volumeEmitter))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 2.0f;
        particles->EmissionDuration = 1.0f;
        particles->EmissionRate = 4.0f;
        particles->ParticleVelocity = { -0.05f, -0.05f, -0.05f };
        particles->ParticleVelocityVariance = { 0.1f, 0.1f, 0.1f };
        particles->StartSize = 0.3f;
        //particles->EndSize = 1.0f;
        particles->bAnimateSize = false;
        particles->EndRotation = 0.0f;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateAbility1Particles(Node* parentNode, Vector3 position, float scale, DirectX::XMFLOAT4 rotation)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability1Particles", nullptr);
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 4.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XY;
        pCircleLogic->ParticlesPerSecond = 100.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\pchniecie.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* particles = pNewNode->GetComponent<ParticleSystemComponent>();
        particles->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        particles->destroyAfterEmission = true;
        particles->ParticleLifetime = 0.7f; // 0.7f
        particles->EmissionDuration = 0.1f;
        //particles->EmissionRate = 40.0f;
        particles->ParticleVelocity = { 0.0f, 0.0f, 1.0f };
        particles->ParticleVelocityVariance = { 0.0f, 0.0f, 70.0f };
        particles->EmitterPositionOffset = { 0.0f, 0.0f, 5.0f };
        particles->StartSize = 2.0f;
        particles->EndSize = 1.0f;
        particles->EndRotation = 0.0f;
        particles->lockRotationOnYAxis = true;
        particles->textureAtlasColumns = 2;
        particles->textureAtlasRows = 2;
        particles->bUseMidColor = true;
        particles->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        particles->ColorMidpoint = 0.1f;
        particles->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        particles->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };

        particles->Play();
        particles->Link(*rg);

        pNewNodeOwner->SetLocalPosition(position);
        pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
        pNewNodeOwner->SetLocalRotation(rotation);

        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

#pragma endregion

#pragma region ENEMIES


    ///////////////////////////////
    ////////////ENEMIES////////////
    ///////////////////////////////

    static Node* InstantiateAnimationnTest(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("AnimationTest", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\mag\\mage.gltf", 1.0f, true)
        );
        pNewNode->AddComponent(
            std::make_unique<AnimationComponent>(pNewNode, "", "Models\\mag\\mage.gltf")
        );

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::make_unique<PointEmitterLogic>())
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        pParticleSystem->ParticleLifetime = 2.0f;
        pParticleSystem->BurstAmount = 20;
        pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { -10.0f, -10.0f, -10.0f };
        pParticleSystem->ParticleVelocityVariance = { 20.0f, 20.0f, 20.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 4.0f;
        pParticleSystem->StartSizeVariance = 10.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;


        AnimationComponent* animComp = pNewNode->GetComponent<AnimationComponent>();
        animComp->PlayAnimation(3);
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateAnimationTest(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("AnimationTest", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        /*pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\char_basic2.glb", 1.0f, true)
        );*/
        /*pNewNode->AddComponent(
            std::make_unique<AnimationComponent>(pNewNode, "", "Models\\char_basic2.glb")
        );*/

        auto sphereEmitter = std::make_unique<SphereToCenterEmitterLogic>();

        // Make the implosion very chaotic
        sphereEmitter->SpeedRandomness = 0.0f;
        sphereEmitter->SpawnRadius = 250.0f;
        sphereEmitter->TravelSpeed = 80.0f;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::move(sphereEmitter))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        pParticleSystem->ParticleLifetime = 3.0f;      
        //pParticleSystem->BurstAmount = 200;
        pParticleSystem->EmissionDuration = 5.0f;
        //pParticleSystem->bUseLifetimeRange = true;
        //pParticleSystem->MinLifetime = 0.1f;
        //pParticleSystem->MaxLifetime = 0.4f;
        //pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f }; 
        /*pParticleSystem->ParticleVelocity = { -30.0f, -30.0f, -30.0f };
        pParticleSystem->ParticleVelocityVariance = { 60.0f, 60.0f, 60.0f };*/
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 0.6f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 4.0f;
        pParticleSystem->StartSizeVariance = 4.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;


        /*AnimationComponent* animComp = pNewNode->GetComponent<AnimationComponent>();
        animComp->PlayAnimation(3);
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateJumpPadActivationParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("JumpPadActivationParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        auto sphereEmitter = std::make_unique<SphereToCenterEmitterLogic>();

        // Make the implosion very chaotic
        sphereEmitter->SpeedRandomness = 0.0f;
        sphereEmitter->SpawnRadius = 250.0f;
        sphereEmitter->TravelSpeed = 80.0f;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::move(sphereEmitter))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        pParticleSystem->ParticleLifetime = 3.0f;
        //pParticleSystem->BurstAmount = 200;
        pParticleSystem->EmissionDuration = 5.0f;
        //pParticleSystem->bUseLifetimeRange = true;
        //pParticleSystem->MinLifetime = 0.1f;
        //pParticleSystem->MaxLifetime = 0.4f;
        //pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        /*pParticleSystem->ParticleVelocity = { -30.0f, -30.0f, -30.0f };
        pParticleSystem->ParticleVelocityVariance = { 60.0f, 60.0f, 60.0f };*/
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 0.6f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 4.0f;
        pParticleSystem->StartSizeVariance = 4.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateEnemyExplodeParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("EnemyExplodeParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::make_unique<PointEmitterLogic>())
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        //pParticleSystem->ParticleLifetime = 0.6f;      
        pParticleSystem->BurstAmount = 150;
        pParticleSystem->bUseLifetimeRange = true;
        pParticleSystem->MinLifetime = 0.1f;
        pParticleSystem->MaxLifetime = 0.4f;
        pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { -30.0f, -30.0f, -30.0f };
        pParticleSystem->ParticleVelocityVariance = { 60.0f, 60.0f, 60.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 0.3f, 0.3f, 0.3f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 0.3f, 0.3f, 0.3f, 1.0f };
        pParticleSystem->EndColor = { 0.3f, 0.3f, 0.3f, 0.0f };
        pParticleSystem->StartSize = 2.0f;
        pParticleSystem->StartSizeVariance = 4.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateEnemyKillParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("EnemyKillParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::make_unique<PointEmitterLogic>())
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        //pParticleSystem->ParticleLifetime = 0.6f;      
        pParticleSystem->BurstAmount = 12;
        pParticleSystem->bUseLifetimeRange = true;
        pParticleSystem->MinLifetime = 0.3f;
        pParticleSystem->MaxLifetime = 1.2f;
        pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { -5.0f, -5.0f, -5.0f };
        pParticleSystem->ParticleVelocityVariance = { 10.0f, 10.0f, 10.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 4.0f;
        pParticleSystem->StartSizeVariance = 7.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateJumpPadParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("JumpPadParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 10.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 8.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\long.png", 10000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        pParticleSystem->ParticleLifetime = 1.0f;
        //pParticleSystem->BurstAmount = 200;
        //pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocityVariance = { 0.0f, 70.0f, 0.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 10.0f;
        pParticleSystem->StartSizeVariance = 10.0f;
        pParticleSystem->lockRotationOnYAxis = true;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateJumpPadSmokeParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("JumpPadSmokeParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 10.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 10.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::move(pCircleLogic))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::Loop);
        pParticleSystem->ParticleLifetime = 1.0f;
        //pParticleSystem->BurstAmount = 200;
        //pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocityVariance = { 0.0f, 70.0f, 0.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.1f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 0.5f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 10.0f;
        pParticleSystem->StartSizeVariance = 10.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateMushroomExplosionParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("MushroomExplosionParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\flame.png", 10000, std::make_unique<PointEmitterLogic>())
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        pParticleSystem->ParticleLifetime = 1.0f;
        pParticleSystem->BurstAmount = 200;
        pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { -15.0f, -15.0f, -15.0f };
        pParticleSystem->ParticleVelocityVariance = { 30.0f, 30.0f, 30.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.008f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 1.0f;
        pParticleSystem->StartSizeVariance = 2.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        //pParticleSystem->textureAtlasRows = 2;
        //pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateMushroomSmokeParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("MushroomSmokeParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        auto volumeEmitter = std::make_unique<SphereVolumeEmitterLogic>();

        volumeEmitter->SpawnRadius = 10.0f;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\fat.png", 10000, std::move(volumeEmitter))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        pParticleSystem->ParticleLifetime = 10.0f;
        pParticleSystem->BurstAmount = 8;
        pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 1.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { -0.5f, -0.5f, -0.5f };
        pParticleSystem->ParticleVelocityVariance = { 1.0f, 1.0f, 1.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 0.67f, 0.61f, 0.29f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.008f;
        pParticleSystem->MidColor = { 0.67f, 0.61f, 0.29f, 0.5f };
        pParticleSystem->EndColor = { 0.67f, 0.61f, 0.29f, 0.0f };
        pParticleSystem->StartSize = 10.0f;
        pParticleSystem->StartSizeVariance = 4.0f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateMushroomParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("MushroomParticles", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        /*pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\char_basic2.glb", 1.0f, true)
        );*/
        /*pNewNode->AddComponent(
            std::make_unique<AnimationComponent>(pNewNode, "", "Models\\char_basic2.glb")
        );*/

        auto volumeEmitter = std::make_unique<SphereVolumeEmitterLogic>();

        volumeEmitter->SpawnRadius = 10.0f;
        volumeEmitter->ParticlesPerSecond = 48000.0f;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\grzyb.png", 10000, std::move(volumeEmitter))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->SetPlaybackMode(ParticleSystemComponent::PlaybackMode::OneShot);
        pParticleSystem->ParticleLifetime = 10.0f;
        pParticleSystem->BurstAmount = 200;
        pParticleSystem->bOneShotIsBurst = true;
        pParticleSystem->EmissionDuration = 0.005f;
        pParticleSystem->destroyAfterEmission = true;
        pParticleSystem->EmitterPositionOffset = { 0.0f, 1.0f, 0.0f };
        pParticleSystem->ParticleVelocity = { -0.5f, -0.5f, -0.5f };
        pParticleSystem->ParticleVelocityVariance = { 1.0f, 1.0f, 1.0f };
        pParticleSystem->bUseMidColor = true;
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->ColorMidpoint = 0.005f;
        pParticleSystem->MidColor = { 1.0f, 1.0f, 1.0f, 0.6f };
        pParticleSystem->EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
        pParticleSystem->StartSize = 0.5f;
        pParticleSystem->StartSizeVariance = 1.5f;
        pParticleSystem->bAnimateSize = false;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f;
        pParticleSystem->textureAtlasRows = 2;
        pParticleSystem->textureAtlasColumns = 2;


        /*AnimationComponent* animComp = pNewNode->GetComponent<AnimationComponent>();
        animComp->PlayAnimation(3);
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);*/
        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pParticleSystem->Play();

        return pNewNode;
    }

    static Node* InstantiateFireplaceParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("AnimationTest", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 1.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 50.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\flame.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->EmissionRate = 100.0f;              // Particles per second
        pParticleSystem->ParticleLifetime = 1.0f;            // How long each particle lives
        pParticleSystem->EmitterPositionOffset = { 0.0f, 1.0f, 0.0f }; // Start slightly above the node's origin
        pParticleSystem->ParticleVelocity = { 0.0f, 1.0f, 0.0f }; // Strong upward velocity
        pParticleSystem->ParticleVelocityVariance = { 0.0f, 4.0f, 0.0f }; // Spread them out horizontally
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // Bluish water color
        pParticleSystem->EndColor = { 0.0f, 0.0f, 0.0f, 1.0f };   // Fade to a light blue and then disappear
        pParticleSystem->StartSize = 0.7f;
        pParticleSystem->EndSize = 0.0f;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f; // Two full rotations over its lifetime
        pParticleSystem->StartSizeVariance = 2.0f;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        return pNewNode;
    }

    static Node* InstantiateCharacterOnFireParticles(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("AnimationTest", nullptr, "ENEMY");
        Node* pNewNode = pNewNodeOwner.get();

        auto pCircleLogic = std::make_unique<CircleEmitterLogic>();
        pCircleLogic->Radius = 1.0f;
        pCircleLogic->Orientation = CircleEmitterLogic::Plane::XZ;
        pCircleLogic->ParticlesPerSecond = 50.0f;
        pCircleLogic->bFill = true;

        pNewNode->AddComponent(
            std::make_unique<ParticleSystemComponent>(pNewNode, wind->Gfx(), "Models\\flame.png", 200, std::move(pCircleLogic))
        );
        ParticleSystemComponent* pParticleSystem = pNewNode->GetComponent<ParticleSystemComponent>();
        pParticleSystem->EmissionRate = 100.0f;              // Particles per second
        pParticleSystem->ParticleLifetime = 1.0f;            // How long each particle lives
        pParticleSystem->EmitterPositionOffset = { 0.0f, 1.0f, 0.0f }; // Start slightly above the node's origin
        pParticleSystem->ParticleVelocity = { 0.0f, 5.0f, 0.0f }; // Strong upward velocity
        pParticleSystem->ParticleVelocityVariance = { 0.0f, 10.0f, 0.0f }; // Spread them out horizontally
        pParticleSystem->StartColor = { 1.0f, 1.0f, 1.0f, 1.0f }; // Bluish water color
        pParticleSystem->EndColor = { 0.0f, 0.0f, 0.0f, 1.0f };   // Fade to a light blue and then disappear
        pParticleSystem->StartSize = 0.7f;
        pParticleSystem->EndSize = 0.0f;
        pParticleSystem->StartRotation = 0.0f;
        pParticleSystem->EndRotation = 0.0f; // Two full rotations over its lifetime
        pParticleSystem->StartSizeVariance = 2.0f;

        pParticleSystem->Link(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        pNewNode->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        return pNewNode;
    }

    static Node* InstantiateNormalEnemy(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Basic", nullptr, "ENEMY");
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
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\basic_gltf\\basic1.gltf", 1.0f, true)
        );

        pNewNode->AddComponent(
            std::make_unique<AnimationComponent>(pNewNode, "", "Models\\basic_gltf\\basic1.gltf")
        );
        AnimationComponent* animComp = pNewNode->GetComponent<AnimationComponent>();
        animComp->PlayAnimation(3);


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

        // ATTACK
        auto attackNodeOwner = std::make_unique<Node>("Basic Attack", nullptr, "TRIGGER");
        Node* pattackNode = attackNodeOwner.get();

        BodyCreationSettings a1BodySettings(new JPH::CapsuleShape(5.0f, 3.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pattackNode->AddComponent(
            std::make_unique<Trigger>(pattackNode, a1BodySettings, false)
        );
        pattackNode->AddComponent(
            std::make_unique<BasicAttack>(pattackNode)
        );
        BasicAttack* basicAttack = pattackNode->GetComponent<BasicAttack>();
        basicAttack->attackRange = 7.0f;
        pNewNode->SetLocalPosition({ 0.0f, 0.0f, 0.0f });
        pNewNode->AddChild(std::move(attackNodeOwner));

        //MOVEMENT
        pNewNode->AddComponent(
            std::make_unique<Walking>(pNewNode)
        );
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 1.5f;
        walking->maxSpeed = 90.0f;
        walking->height = 7.2f;


        //STATE MACHINE
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        StateMachine* stateMachine = pNewNode->GetComponent<StateMachine>();
        stateMachine->enemyType = EnemyType::BASIC;
        stateMachine->followDistance = 60.0f;
        stateMachine->pPlayer = player;
        stateMachine->attackComponents.push_back(basicAttack);
        stateMachine->pMovementComponent = walking;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );
        

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));



        return pNewNode;
    }
    static Node* InstantiateTankEnemy(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Basic", nullptr, "ENEMY");
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
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\Tank\\tank.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(2.1f, 1.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::ENEMY);
        eBodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
        eBodySettings.mMassPropertiesOverride.mMass = 15.0f;
        eBodySettings.mFriction = 0.2f;
        eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, eBodySettings)
        );

        // ATTACK
        auto attackNodeOwner = std::make_unique<Node>("Basic Attack", nullptr, "TRIGGER");
        Node* pattackNode = attackNodeOwner.get();

        BodyCreationSettings a1BodySettings(new JPH::CapsuleShape(5.0f, 3.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pattackNode->AddComponent(
            std::make_unique<Trigger>(pattackNode, a1BodySettings, false)
        );
        pattackNode->AddComponent(
            std::make_unique<BasicAttack>(pattackNode)
        );
        BasicAttack* basicAttack = pattackNode->GetComponent<BasicAttack>();
        basicAttack->attackRange = 7.0f;
        pNewNode->SetLocalPosition({ 0.0f, 0.0f, 0.0f });
        pNewNode->AddChild(std::move(attackNodeOwner));

        //MOVEMENT
        pNewNode->AddComponent(
            std::make_unique<Walking>(pNewNode)
        );
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 1.5f;
        walking->maxSpeed = 90.0f;


        //STATE MACHINE
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        StateMachine* stateMachine = pNewNode->GetComponent<StateMachine>();
        stateMachine->followDistance = 60.0f;
        stateMachine->pPlayer = player;
        stateMachine->attackComponents.push_back(basicAttack);
        stateMachine->pMovementComponent = walking;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );
        Health* health = pNewNode->GetComponent<Health>();
        //health->tank = true;


        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));



        return pNewNode;
    }
    static Node* InstantiateFlyingEnemy(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Flying", nullptr, "ENEMY");
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


        // ATTACK
        pNewNode->AddComponent(
            std::make_unique<ShootAttack>(pNewNode, player)
        );
        ShootAttack* shootAttack = pNewNode->GetComponent<ShootAttack>();
        shootAttack->bulletSpeed = 40.0f;
        shootAttack->attackRange = 60.0f;

        //MOVEMENT
        pNewNode->AddComponent(
            std::make_unique<Flying>(pNewNode)
        );
        Flying* flying = pNewNode->GetComponent<Flying>();
        flying->maxSpeed = 38.0f;
        flying->height = 2.0f;

        //STATE MACHINE
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        StateMachine* stateMachine = pNewNode->GetComponent<StateMachine>();
        stateMachine->followDistance = 120.0f;
        stateMachine->isFlying = true;
        stateMachine->pPlayer = player;
        stateMachine->attackComponents.push_back(shootAttack);
        stateMachine->pMovementComponent = flying;

        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }
    static Node* InstantiateShootingEnemy(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Shooting", nullptr, "ENEMY");
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
        eBodySettings.mMassPropertiesOverride.mMass = 7.0f;
        eBodySettings.mFriction = 0.2f;
        eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, eBodySettings)
        );

        // ATTACK
        pNewNode->AddComponent(
            std::make_unique<ShootAttack>(pNewNode, player)
        );
        ShootAttack* shootAttack = pNewNode->GetComponent<ShootAttack>();
        shootAttack->bulletSpeed = 50.0f;
        shootAttack->attackRange = 60.0f;

        //MOVEMENT
        auto voidOwner = std::make_unique<Node>("VoidCheck", nullptr, "TRIGGER");
        voidOwner->AddComponent(
            std::make_unique<ModelComponent>(voidOwner.get(), wind->Gfx(), "Models\\box.glb")
        );
        voidOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        pNewNode->AddComponent(
            std::make_unique<Walking>(pNewNode)
        );
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 1.5f;
        walking->maxSpeed = 80.0f;
        walking->height = 5.4f;
        //walking->voidNode = voidOwner.get();
        root->AddChild(std::move(voidOwner));



        //STATE MACHINE
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        StateMachine* stateMachine = pNewNode->GetComponent<StateMachine>();
        stateMachine->followDistance = 90.0f;
        stateMachine->pPlayer = player;
        stateMachine->attackComponents.push_back(shootAttack);
        stateMachine->pMovementComponent = walking;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        return pNewNode;
    }
    static Node* InstantiateMageEnemy(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Mage", nullptr, "ENEMY");
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
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\Mage\\mage.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(3.8f, 2.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::ENEMY);
        eBodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
        eBodySettings.mMassPropertiesOverride.mMass = 100.0f;
        eBodySettings.mFriction = 0.2f;
        eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, eBodySettings)
        );

        // ATTACK
        pNewNode->AddComponent(
            std::make_unique<SpawnAttack>(pNewNode, player)
        );
        SpawnAttack* spawnAttack = pNewNode->GetComponent<SpawnAttack>();
        spawnAttack->attackRange = 150.0f;
        spawnAttack->cooldownTime = 20.0f;
        

        //MOVEMENT
        pNewNode->AddComponent(
            std::make_unique<Walking>(pNewNode)
        );
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 2.5f;
        walking->maxSpeed = 1.0f;
		walking->height = 12.6f;


        //STATE MACHINE
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        StateMachine* stateMachine = pNewNode->GetComponent<StateMachine>();
        stateMachine->followDistance = 60.0f;
        stateMachine->pPlayer = player;
        stateMachine->attackComponents.push_back(spawnAttack);
        stateMachine->pMovementComponent = walking;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );
        Health* health = pNewNode->GetComponent<Health>();
        //health->tank = true;



        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));



        return pNewNode;
    }
    static Node* InstantiateExplosiveEnemy(Node* parentNode, Vector3 position, float scale)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Explosive", nullptr, "ENEMY");
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
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\enemy\\Explosive\\explosive.obj")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);
        parentNode->AddChild(std::move(pNewNodeOwner));

        BodyCreationSettings eBodySettings(new JPH::CapsuleShape(0.5f, 1.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::ENEMY);
        eBodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

        //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
        eBodySettings.mMassPropertiesOverride.mMass = 5.0f;
        eBodySettings.mFriction = 0.2f;
        eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
        eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
        pNewNode->AddComponent(
            std::make_unique<Rigidbody>(pNewNode, eBodySettings)
        );


        // ATTACK
        auto attackNodeOwner = std::make_unique<Node>("Boom Attack", nullptr, "TRIGGER");
        Node* pattackNode = attackNodeOwner.get();

        BodyCreationSettings a1BodySettings(new JPH::SphereShape(10.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pattackNode->AddComponent(
            std::make_unique<Trigger>(pattackNode, a1BodySettings, false)
        );
        pattackNode->AddComponent(
            std::make_unique<BoomAttack>(pattackNode, player)
        );
        BoomAttack* boomAttack = pattackNode->GetComponent<BoomAttack>();
        boomAttack->attackRange = 30.0f;
        boomAttack->boomTime = 3.0f;
        boomAttack->knockRange = 11.0f;
        pNewNode->SetLocalPosition({ 0.0f, 0.0f, 0.0f });
        pNewNode->AddChild(std::move(attackNodeOwner));

        //MOVEMENT
        pNewNode->AddComponent(
            std::make_unique<Walking>(pNewNode)
        );
        Walking* walking = pNewNode->GetComponent<Walking>();
        walking->radius = 1.0f;
        walking->maxSpeed = 40.0f;
        walking->height = 4.0f;


        //STATE MACHINE
        pNewNode->AddComponent(
            std::make_unique<StateMachine>(pNewNode, StateType::IDLE)
        );
        StateMachine* stateMachine = pNewNode->GetComponent<StateMachine>();
        stateMachine->followDistance = 60.0f;
        stateMachine->pPlayer = player;
        stateMachine->attackComponents.push_back(boomAttack);
        stateMachine->pMovementComponent = walking;
        stateMachine->canDropPills = false;
        pNewNode->AddComponent(
            std::make_unique<Health>(pNewNode, 1.0f)
        );
        Health* health = pNewNode->GetComponent<Health>();
        //health->tank = true;



        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));



        return pNewNode;
    }

    static Node* InstantiateHealthCollectable(Node* parentNode, Vector3 position, float scale, float targetY)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Health", nullptr, "TRIGGER");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        BodyCreationSettings bodySettings(new JPH::SphereShape(3.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, bodySettings, false)
        );

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pNewNode->AddComponent(
            std::make_unique<Collectable>(pNewNode, targetY)
        );
        pNewNode->GetComponent<Collectable>()->health = true;




        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateExpCollectable(Node* parentNode, Vector3 position, float scale, float targetY)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Exp", nullptr, "TRIGGER");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        BodyCreationSettings bodySettings(new JPH::SphereShape(3.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, bodySettings, false)
        );
        
        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

        pNewNode->AddComponent(
            std::make_unique<Collectable>(pNewNode, targetY)
        );
        //pNewNode->GetComponent<Collectable>()->health = false;




        parentNode->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }
    static Node* InstantiateAbility5Extend(Node* parentNode, Vector3 position, float scale, float force, float duration)
    {
        auto pNewNodeOwner = std::make_unique<Node>("Ability5Extend", nullptr, "TRIGGER");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb")
        );
        pNewNodeOwner->GetComponent<ModelComponent>()->LinkTechniques(*rg);

        BodyCreationSettings bodySettings(new JPH::CapsuleShape(30.0f, 5.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
        pNewNode->AddComponent(
            std::make_unique<Trigger>(pNewNode, bodySettings, false)
        );
        pNewNode->AddComponent(
            std::make_unique<Ability5Extend>(pNewNode, force, duration)
        );

        pNewNode->SetLocalPosition(position);
        pNewNode->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));


        root->AddChild(std::move(pNewNodeOwner));

        return pNewNode;
    }

    static Node* InstantiateBullet(Vector3 position, float scale) 
    {
        auto pNewNodeOwner = std::make_unique<Node>("Bullet", nullptr, "BULLET");
        Node* pNewNode = pNewNodeOwner.get();


        pNewNode->AddComponent(
            std::make_unique<ModelComponent>(pNewNode, wind->Gfx(), "Models\\box.glb", 1.0f)
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

#pragma endregion

#pragma region PREFAB UTILITY

    ///////////////////////////////
    ////////PREFAB UTILITY/////////
    ///////////////////////////////

    static void InstantiateThrowable(Node* parentNode, Vector3 position, float scale, Vector3 rotation = { 0,0,0 }) {
        int randomIndex = rand() % 5 + 1;

        switch (randomIndex) {
        case 1:
            InstantiateStone1(parentNode, position, scale, rotation);
            break;
        case 2:
            InstantiateStone2(parentNode, position, scale, rotation);
            break;
        case 3:
            InstantiateStone3(parentNode, position, scale, rotation);
            break;
        case 4:
            InstantiateStone4(parentNode, position, scale, rotation);
            break;
        case 5:
            InstantiateStone5(parentNode, position, scale, rotation);
            break;
        }
    }
    Window* wnd;
private:
};

#pragma endregion

