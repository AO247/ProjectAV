#include "PrefabManager.h"
#include "Components.h"
#include "PhysicsCommon.h"

Window* PrefabManager::wind = nullptr;
Node* PrefabManager::root = nullptr;

PrefabManager::~PrefabManager()
{
}

Node* PrefabManager::InstantiateMushroom1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Mushroom1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\grzyb_1.2.obj")
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

    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateMushroom2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\grzyb_2.2.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateMushroom3(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\grzyb_3.2.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}


Node* PrefabManager::InstantiateWall(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_5.3.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateStone2(Node* parentNode, float locX, float locY, float locZ, float scale) const {
    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\kamien_2.2.obj")
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

Node* PrefabManager::InstantiateStone3(Node* parentNode, float locX, float locY, float locZ, float scale) const {
    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\kamien_3.2.obj")
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

Node* PrefabManager::InstantiateStone4(Node* parentNode, float locX, float locY, float locZ, float scale) const {
    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\kamien_4.2.obj")
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

Node* PrefabManager::InstantiateStone5(Node* parentNode, float locX, float locY, float locZ, float scale) const {
    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "STONE");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male_2\\kamien_5.2.obj")
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

Node* PrefabManager::InstantiatePlatform1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_1.2.obj")
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

    
    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiatePlatform2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_2.2.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiatePlatform3(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_3.3.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiatePlatform4(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_4.3.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiatePlatform5(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_5.3.obj")
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


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiatePlatform6(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Platform1", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy_2\\platforma_6.2.obj")
    );
   /* ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
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
    );*/


    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}



Node* PrefabManager::InstantiateIslandBig2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "GROUND");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_duza_prostokat_2.obj")
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

    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-64.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(84.0f, 0.0f, -10.0f));
    upPoint->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 50.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -53.0f));
    //pIsland->halfExtents = { 24.0f, 2.0f, 24.0f };
    //pIsland->halfExtents = { 10.0f, 2.0f, 10.0f };

    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                            2.0f,
                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
 /*   upPoint->AddComponent(
        std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    downPoint->AddComponent(
        std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    leftPoint->AddComponent(
        std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    rightPoint->AddComponent(
        std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );*/
    pIsland->leftPoint = leftPoint.get();
    pIsland->rightPoint = rightPoint.get();
    pIsland->upPoint = upPoint.get();
    pIsland->downPoint = downPoint.get();
    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();


    //Node* stone1 = InstantiateStone1(pNewNode, 22.2f, 30.0f, 32.2f, 2.0f);
    //Node* stone2 = InstantiateStone2(pNewNode, 27.0f, 30.0f, 17.2f, 2.0f);
    //Node* stone3 = InstantiateStone2(pNewNode, 27.2f, 30.0f, -2.2f, 2.0f);
    Node* stone4 = InstantiateStone3(pNewNode, -8.2f, 30.0f, -18.2f, 2.0f);
    Node* stone5 = InstantiateStone4(pNewNode, 15.2f, 30.0f, 4.2f, 2.0f);
    Node* stone6 = InstantiateStone5(pNewNode, 11.2f, 30.0f, -8.2f, 2.0f);
    Node* column1 = InstantiateColumn(pNewNode, 45.0f, 0.0f, -25.0f, 1.0f);
    Node* column2 = InstantiateColumn(pNewNode, 10.0f, 0.0f, 25.0f, 1.0f);
    Node* column3 = InstantiateColumn(pNewNode, 29.0f, 0.0f, -20.0f, 1.0f);
    Node* column4 = InstantiateColumn(pNewNode, 2.0f, 0.0f, -16.0f, 1.0f);
    Node* platform1 = InstantiatePlatform1(pNewNode, 50.0f, 0.0f, 0.0f, 1.0f);
    Node* platform2 = InstantiatePlatform5(pNewNode, -28.0f, 0.0f, 6.0f, 1.0f);


    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
    auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(23.0f, 2.0f, 33.0f));
    spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(28.0f, 2.0f, 18.0f));
    spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(28.0f, 2.0f, -1.0f));
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

Node* PrefabManager::InstantiateIslandMedium1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island M1", nullptr, "GROUND");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_srednia_2.obj")
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

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 38.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -43.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-47.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(47.0f, 0.0f, -4.0f));
    //pIsland->halfExtents = { 18.0f, 2.0f, 17.0f };
    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x)/2.0f - 1.0f,
                            2.0f,
                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z)/2.0f - 1.0f };
 /*   upPoint->AddComponent(
        std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    downPoint->AddComponent(
        std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    leftPoint->AddComponent(
        std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    rightPoint->AddComponent(
        std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );*/
    pIsland->leftPoint = leftPoint.get();
    pIsland->rightPoint = rightPoint.get();
    pIsland->upPoint = upPoint.get();
    pIsland->downPoint = downPoint.get();

    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();

    //Node* stone1 = InstantiateStone1(pNewNode, -7.2f, 15.0f, 23.2f, 2.0f);
    Node* stone2 = InstantiateStone2(pNewNode, 31.0f, 30.0f, 15.2f, 2.0f);
    Node* stone3 = InstantiateStone2(pNewNode, 17.2f, 30.0f, -2.2f, 2.0f);
    Node* stone4 = InstantiateStone3(pNewNode, -15.2f, 30.0f, -25.2f, 2.0f);
    Node* stone5 = InstantiateStone4(pNewNode, 10.2f, 30.0f, -22.2f, 2.0f);
    Node* stone6 = InstantiateStone5(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
    Node* column1 = InstantiateColumn(pNewNode, 24.5f, 0.0f, -7.0f, 1.0f);
    Node* column2 = InstantiateColumn(pNewNode, -4.5f, 0.0f, -20.0f, 1.0f);
    Node* column3 = InstantiateColumn(pNewNode, -18.5f, 8.4f, 15.0f, 1.0f);
	Node* platform1 = InstantiatePlatform2(pNewNode, -17.0f, 0.0f, 10.0f, 1.0f);

    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(15.0f, 2.0f, 2.0f));
    pIsland->spawnPoints.push_back(spawnPoint1.get());
    pNewNode->AddChild(std::move(spawnPoint1));

    pNewNode->AddChild(std::move(leftPoint));
    pNewNode->AddChild(std::move(rightPoint));
    pNewNode->AddChild(std::move(upPoint));
    pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateIslandMedium2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island M2", nullptr, "GROUND");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_srednia_1.obj")
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

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 13.5f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -14.5f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-15.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(19.0f, 0.0f, -5.0f));
    //pIsland->halfExtents = { 18.0f, 2.0f, 15.0f };
    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                            2.0f,
                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
    /*upPoint->AddComponent(
		std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
	);
	downPoint->AddComponent(
		std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
	);
	leftPoint->AddComponent(
		std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
	);
	rightPoint->AddComponent(
		std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
	);*/

    pIsland->leftPoint = leftPoint.get();
    pIsland->rightPoint = rightPoint.get();
    pIsland->upPoint = upPoint.get();
    pIsland->downPoint = downPoint.get();

    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();

    //Node* stone = InstantiateStone(pNewNode, pNewNode->GetWorldPosition().x + 5.0f, 30.0f, 0.0f, 1.5f);
    //Node* column1 = InstantiateColumn(pNewNode, 5.0f, 0.0f, -5.0f, 1.0f);
    //Node* column2 = InstantiateColumn(pNewNode, -5.0f, 0.0f, 5.0f, 1.0f);

    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 2.0f, -3.0f));
    spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-3.0f, 2.0f, 3.0f));
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

Node* PrefabManager::InstantiateIslandSmall1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_mala_2.obj")
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

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 35.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -37.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
    //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
                            2.0f,
                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
 /*   upPoint->AddComponent(
        std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    downPoint->AddComponent(
        std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    leftPoint->AddComponent(
        std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    rightPoint->AddComponent(
        std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );*/
    pIsland->leftPoint = leftPoint.get();
    pIsland->rightPoint = rightPoint.get();
    pIsland->upPoint = upPoint.get();
    pIsland->downPoint = downPoint.get();
    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();

    //Node* stone1 = InstantiateStone1(pNewNode, -7.2f, 15.0f, 8.2f, 2.0f);
    Node* stone2 = InstantiateStone2(pNewNode, -2.0f, 30.0f, 0.0f, 2.0f);
    Node* stone3 = InstantiateStone2(pNewNode, 12.2f, 30.0f, 4.2f, 2.0f);
    Node* stone4 = InstantiateStone3(pNewNode, -12.2f, 30.0f, -21.2f, 2.0f);
    Node* stone5 = InstantiateStone4(pNewNode, 12.2f, 30.0f, -20.2f, 2.0f);
    Node* stone6 = InstantiateStone5(pNewNode, -16.2f, 30.0f, 1.2f, 2.0f);
    Node* column1 = InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
    Node* column2 = InstantiateColumn(pNewNode, -6.5f, 0.0f, -15.0f, 1.0f);
    Node* column3 = InstantiateColumn(pNewNode, 7.5f, 0.0f, 10.0f, 1.0f);

    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-8.0f, 2.0f, 9.0f));
    pIsland->spawnPoints.push_back(spawnPoint1.get());
    pNewNode->AddChild(std::move(spawnPoint1));

    pNewNode->AddChild(std::move(leftPoint));
    pNewNode->AddChild(std::move(rightPoint));
    pNewNode->AddChild(std::move(upPoint));
    pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateIslandSmall2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_mala_L_2.obj")
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
    Node* stone2 = InstantiateStone2(pNewNode, 3.0f, 30.0f, -8.0f, 2.0f);
    Node* stone3 = InstantiateStone2(pNewNode, 20.2f, 30.0f, 4.2f, 2.0f);
    Node* stone4 = InstantiateStone3(pNewNode, 15.2f, 30.0f, 21.2f, 2.0f);
    Node* stone5 = InstantiateStone4(pNewNode, 12.2f, 30.0f, -3.2f, 2.0f);
    Node* stone6 = InstantiateStone5(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
    Node* column1 = InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
    Node* column2 = InstantiateColumn(pNewNode, -4.5f, 0.0f, 1.0f, 1.0f);
    Node* column3 = InstantiateColumn(pNewNode, 16.5f, 0.0f, 10.0f, 1.0f);
    
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



Node* PrefabManager::InstantiateEnemy(Node* parentNode, float locX, float locY, float locZ, float scale, Node* pPlayer) const {


    auto pNewNodeOwner = std::make_unique<Node>("Enemy", nullptr, "ENEMY");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enemy\\basic2.obj")
    );
    BodyCreationSettings eBodySettings(new JPH::CapsuleShape(2.1f, 1.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
    eBodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
    eBodySettings.mMassPropertiesOverride.mMass = 10.0f;
    eBodySettings.mFriction = 0.2f;
    eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
	eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
    pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), eBodySettings)
    );

    BodyCreationSettings a1BodySettings(new JPH::CapsuleShape(5.0f, 3.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pNewNodeOwner->AddComponent(
        std::make_unique<Trigger>(pNewNodeOwner.get(), a1BodySettings, false)
    );
    pNewNodeOwner->AddComponent(
        std::make_unique<Walking>(pNewNodeOwner.get())
    );
    pNewNodeOwner->AddComponent(
        std::make_unique<BasicAttack>(pNewNodeOwner.get())
    );
    pNewNodeOwner->AddComponent(
        std::make_unique<StateMachine>(pNewNodeOwner.get(), StateType::IDLE)
    );
    pNewNodeOwner->GetComponent<StateMachine>()->attackRange = 4.0f;
    pNewNodeOwner->GetComponent<StateMachine>()->followDistance = 40.0f;
	pNewNodeOwner->GetComponent<StateMachine>()->pPlayer = pPlayer;
    pNewNodeOwner->AddComponent(
        std::make_unique<Health>(pNewNodeOwner.get(), 1.0f)
    );

    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));

    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}


Node* PrefabManager::InstantiateShootingEnemy(Node* parentNode, float locX, float locY, float locZ, float scale, Node* pPlayer) const {


    auto pNewNodeOwner = std::make_unique<Node>("Enemy", nullptr, "ENEMY");
    Node* pNewNode = pNewNodeOwner.get();
    parentNode->AddChild(std::move(pNewNodeOwner));

    pNewNode->AddComponent(
        std::make_unique<ModelComponent>(pNewNode, wnd->Gfx(), "Models\\enemy\\basic2.obj")
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
