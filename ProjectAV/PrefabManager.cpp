#include "PrefabManager.h"
#include "Components.h"
#include "PhysicsCommon.h"

Window* PrefabManager::wind = nullptr;
Node* PrefabManager::root = nullptr;
Node* PrefabManager::player = nullptr;
Rgph::MainRenderGraph* PrefabManager::rg = nullptr;

PrefabManager::~PrefabManager()
{
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



//Node* PrefabManager::InstantiateIslandBig2(Node* parentNode, float locX, float locY, float locZ, float scale) const {
//
//
//    auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "GROUND");
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_duza_prostokat_2.obj")
//    );
//    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
//    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
//    MeshShapeSettings islandMeshSettings(islandTriangles);
//    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
//    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
//    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
//    islandMeshShape = islandScaling.Create().Get();
//    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
//    bodySettings.mFriction = 1.0f;
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
//    );
//
//
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Island>(pNewNodeOwner.get())
//    );
//    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
//    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
//    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
//    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
//    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());
//
//    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-64.0f, 0.0f, 5.0f));
//    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(84.0f, 0.0f, -10.0f));
//    upPoint->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 50.0f));
//    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -53.0f));
//    //pIsland->halfExtents = { 24.0f, 2.0f, 24.0f };
//    //pIsland->halfExtents = { 10.0f, 2.0f, 10.0f };
//
//    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
//                            2.0f,
//                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
// /*   upPoint->AddComponent(
//        std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    downPoint->AddComponent(
//        std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    leftPoint->AddComponent(
//        std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    rightPoint->AddComponent(
//        std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );*/
//    pIsland->leftPoint = leftPoint.get();
//    pIsland->rightPoint = rightPoint.get();
//    pIsland->upPoint = upPoint.get();
//    pIsland->downPoint = downPoint.get();
//    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
//    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
//
//    Node* pNewNode = pNewNodeOwner.get();
//
//
//    //Node* stone1 = InstantiateStone1(pNewNode, 22.2f, 30.0f, 32.2f, 2.0f);
//    //Node* stone2 = InstantiateStone2(pNewNode, 27.0f, 30.0f, 17.2f, 2.0f);
//    //Node* stone3 = InstantiateStone2(pNewNode, 27.2f, 30.0f, -2.2f, 2.0f);
//    Node* stone4 = InstantiateStone3(pNewNode, -8.2f, 30.0f, -18.2f, 2.0f);
//    Node* stone5 = InstantiateStone4(pNewNode, 15.2f, 30.0f, 4.2f, 2.0f);
//    Node* stone6 = InstantiateStone5(pNewNode, 11.2f, 30.0f, -8.2f, 2.0f);
//    Node* column1 = InstantiateColumn(pNewNode, 45.0f, 0.0f, -25.0f, 1.0f);
//    Node* column2 = InstantiateColumn(pNewNode, 10.0f, 0.0f, 25.0f, 1.0f);
//    Node* column3 = InstantiateColumn(pNewNode, 29.0f, 0.0f, -20.0f, 1.0f);
//    Node* column4 = InstantiateColumn(pNewNode, 2.0f, 0.0f, -16.0f, 1.0f);
//    Node* platform1 = InstantiatePlatform1(pNewNode, 50.0f, 0.0f, 0.0f, 1.0f);
//    Node* platform2 = InstantiatePlatform5(pNewNode, -28.0f, 0.0f, 6.0f, 1.0f);
//
//
//    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
//    auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
//    auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
//    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(23.0f, 2.0f, 33.0f));
//    spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(28.0f, 2.0f, 18.0f));
//    spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(28.0f, 2.0f, -1.0f));
//    pIsland->spawnPoints.push_back(spawnPoint1.get());
//    pIsland->spawnPoints.push_back(spawnPoint2.get());
//    pIsland->spawnPoints.push_back(spawnPoint3.get());
//    pNewNode->AddChild(std::move(spawnPoint1));
//    pNewNode->AddChild(std::move(spawnPoint2));
//    pNewNode->AddChild(std::move(spawnPoint3));
//
//    pNewNode->AddChild(std::move(leftPoint));
//    pNewNode->AddChild(std::move(rightPoint));
//    pNewNode->AddChild(std::move(upPoint));
//    pNewNode->AddChild(std::move(downPoint));
//    parentNode->AddChild(std::move(pNewNodeOwner));
//    return pNewNode;
//}
//
//Node* PrefabManager::InstantiateIslandMedium1(Node* parentNode, float locX, float locY, float locZ, float scale) const {
//
//
//    auto pNewNodeOwner = std::make_unique<Node>("Island M1", nullptr, "GROUND");
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_srednia_2.obj")
//    );
//    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
//    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
//    MeshShapeSettings islandMeshSettings(islandTriangles);
//    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
//    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
//    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
//    islandMeshShape = islandScaling.Create().Get();
//    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
//    bodySettings.mFriction = 1.0f;
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
//    );
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Island>(pNewNodeOwner.get())
//    );
//    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
//    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
//    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
//    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
//    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());
//
//    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 38.0f));
//    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -43.0f));
//    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-47.0f, 0.0f, 5.0f));
//    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(47.0f, 0.0f, -4.0f));
//    //pIsland->halfExtents = { 18.0f, 2.0f, 17.0f };
//    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x)/2.0f - 1.0f,
//                            2.0f,
//                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z)/2.0f - 1.0f };
// /*   upPoint->AddComponent(
//        std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    downPoint->AddComponent(
//        std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    leftPoint->AddComponent(
//        std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    rightPoint->AddComponent(
//        std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );*/
//    pIsland->leftPoint = leftPoint.get();
//    pIsland->rightPoint = rightPoint.get();
//    pIsland->upPoint = upPoint.get();
//    pIsland->downPoint = downPoint.get();
//
//    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
//    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
//
//    Node* pNewNode = pNewNodeOwner.get();
//
//    //Node* stone1 = InstantiateStone1(pNewNode, -7.2f, 15.0f, 23.2f, 2.0f);
//    Node* stone2 = InstantiateStone2(pNewNode, 31.0f, 30.0f, 15.2f, 2.0f);
//    Node* stone3 = InstantiateStone2(pNewNode, 17.2f, 30.0f, -2.2f, 2.0f);
//    Node* stone4 = InstantiateStone3(pNewNode, -15.2f, 30.0f, -25.2f, 2.0f);
//    Node* stone5 = InstantiateStone4(pNewNode, 10.2f, 30.0f, -22.2f, 2.0f);
//    Node* stone6 = InstantiateStone5(pNewNode, -15.2f, 30.0f, -8.2f, 2.0f);
//    Node* column1 = InstantiateColumn(pNewNode, 24.5f, 0.0f, -7.0f, 1.0f);
//    Node* column2 = InstantiateColumn(pNewNode, -4.5f, 0.0f, -20.0f, 1.0f);
//    Node* column3 = InstantiateColumn(pNewNode, -18.5f, 8.4f, 15.0f, 1.0f);
//	Node* platform1 = InstantiatePlatform2(pNewNode, -17.0f, 0.0f, 10.0f, 1.0f);
//
//    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
//    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(15.0f, 2.0f, 2.0f));
//    pIsland->spawnPoints.push_back(spawnPoint1.get());
//    pNewNode->AddChild(std::move(spawnPoint1));
//
//    pNewNode->AddChild(std::move(leftPoint));
//    pNewNode->AddChild(std::move(rightPoint));
//    pNewNode->AddChild(std::move(upPoint));
//    pNewNode->AddChild(std::move(downPoint));
//    parentNode->AddChild(std::move(pNewNodeOwner));
//    return pNewNode;
//}
//
//Node* PrefabManager::InstantiateIslandMedium2(Node* parentNode, float locX, float locY, float locZ, float scale) const {
//
//
//    auto pNewNodeOwner = std::make_unique<Node>("Island M2", nullptr, "GROUND");
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_srednia_1.obj")
//    );
//    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
//    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
//    MeshShapeSettings islandMeshSettings(islandTriangles);
//    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
//    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
//    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
//    islandMeshShape = islandScaling.Create().Get();
//    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
//    bodySettings.mFriction = 1.0f;
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
//    );
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Island>(pNewNodeOwner.get())
//    );
//    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
//    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
//    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
//    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
//    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());
//
//    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 13.5f));
//    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -14.5f));
//    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-15.0f, 0.0f, 5.0f));
//    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(19.0f, 0.0f, -5.0f));
//    //pIsland->halfExtents = { 18.0f, 2.0f, 15.0f };
//    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
//                            2.0f,
//                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
//    /*upPoint->AddComponent(
//		std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//	);
//	downPoint->AddComponent(
//		std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//	);
//	leftPoint->AddComponent(
//		std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//	);
//	rightPoint->AddComponent(
//		std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//	);*/
//
//    pIsland->leftPoint = leftPoint.get();
//    pIsland->rightPoint = rightPoint.get();
//    pIsland->upPoint = upPoint.get();
//    pIsland->downPoint = downPoint.get();
//
//    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
//    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
//
//    Node* pNewNode = pNewNodeOwner.get();
//
//    //Node* stone = InstantiateStone(pNewNode, pNewNode->GetWorldPosition().x + 5.0f, 30.0f, 0.0f, 1.5f);
//    //Node* column1 = InstantiateColumn(pNewNode, 5.0f, 0.0f, -5.0f, 1.0f);
//    //Node* column2 = InstantiateColumn(pNewNode, -5.0f, 0.0f, 5.0f, 1.0f);
//
//    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
//    auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
//    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 2.0f, -3.0f));
//    spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-3.0f, 2.0f, 3.0f));
//    pIsland->spawnPoints.push_back(spawnPoint1.get());
//    pIsland->spawnPoints.push_back(spawnPoint2.get());
//    pNewNode->AddChild(std::move(spawnPoint1));
//    pNewNode->AddChild(std::move(spawnPoint2));
//
//    pNewNode->AddChild(std::move(leftPoint));
//    pNewNode->AddChild(std::move(rightPoint));
//    pNewNode->AddChild(std::move(upPoint));
//    pNewNode->AddChild(std::move(downPoint));
//    parentNode->AddChild(std::move(pNewNodeOwner));
//    return pNewNode;
//}
//
//Node* PrefabManager::InstantiateIslandSmall1(Node* parentNode, float locX, float locY, float locZ, float scale) const {
//
//
//    auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "GROUND");
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy_2\\wyspa_mala_2.obj")
//    );
//    ModelComponent* islandModel = pNewNodeOwner->GetComponent<ModelComponent>();
//    TriangleList islandTriangles = PhysicsCommon::MakeTriangleList(islandModel->GetAllTriangles());
//    MeshShapeSettings islandMeshSettings(islandTriangles);
//    Shape::ShapeResult islandMeshCreationResult = islandMeshSettings.Create();
//    ShapeRefC islandMeshShape = islandMeshCreationResult.Get();
//    ScaledShapeSettings islandScaling(islandMeshShape, Vec3Arg(scale, scale, scale));
//    islandMeshShape = islandScaling.Create().Get();
//    BodyCreationSettings bodySettings(islandMeshShape, RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::GROUND);
//    bodySettings.mFriction = 1.0f;
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Rigidbody>(pNewNodeOwner.get(), bodySettings)
//    );
//
//    pNewNodeOwner->AddComponent(
//        std::make_unique<Island>(pNewNodeOwner.get())
//    );
//    Island* pIsland = pNewNodeOwner->GetComponent<Island>();
//    auto leftPoint = std::make_unique<Node>("Left Point", pNewNodeOwner.get());
//    auto rightPoint = std::make_unique<Node>("Right Point", pNewNodeOwner.get());
//    auto upPoint = std::make_unique<Node>("Up Point", pNewNodeOwner.get());
//    auto downPoint = std::make_unique<Node>("Down Point", pNewNodeOwner.get());
//
//    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 35.0f));
//    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -37.0f));
//    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-33.0f, 0.0f, 5.0f));
//    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(33.0f, 0.0f, -4.0f));
//    //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
//    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x) / 2.0f - 1.0f,
//                            2.0f,
//                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z) / 2.0f - 1.0f };
// /*   upPoint->AddComponent(
//        std::make_unique<ModelComponent>(upPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    downPoint->AddComponent(
//        std::make_unique<ModelComponent>(downPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    leftPoint->AddComponent(
//        std::make_unique<ModelComponent>(leftPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );
//    rightPoint->AddComponent(
//        std::make_unique<ModelComponent>(rightPoint.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
//    );*/
//    pIsland->leftPoint = leftPoint.get();
//    pIsland->rightPoint = rightPoint.get();
//    pIsland->upPoint = upPoint.get();
//    pIsland->downPoint = downPoint.get();
//    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
//    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
//
//    Node* pNewNode = pNewNodeOwner.get();
//
//    //Node* stone1 = InstantiateStone1(pNewNode, -7.2f, 15.0f, 8.2f, 2.0f);
//    Node* stone2 = InstantiateStone2(pNewNode, -2.0f, 30.0f, 0.0f, 2.0f);
//    Node* stone3 = InstantiateStone2(pNewNode, 12.2f, 30.0f, 4.2f, 2.0f);
//    Node* stone4 = InstantiateStone3(pNewNode, -12.2f, 30.0f, -21.2f, 2.0f);
//    Node* stone5 = InstantiateStone4(pNewNode, 12.2f, 30.0f, -20.2f, 2.0f);
//    Node* stone6 = InstantiateStone5(pNewNode, -16.2f, 30.0f, 1.2f, 2.0f);
//    Node* column1 = InstantiateColumn(pNewNode, 17.5f, 0.0f, -7.0f, 1.0f);
//    Node* column2 = InstantiateColumn(pNewNode, -6.5f, 0.0f, -15.0f, 1.0f);
//    Node* column3 = InstantiateColumn(pNewNode, 7.5f, 0.0f, 10.0f, 1.0f);
//
//    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
//    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(-8.0f, 2.0f, 9.0f));
//    pIsland->spawnPoints.push_back(spawnPoint1.get());
//    pNewNode->AddChild(std::move(spawnPoint1));
//
//    pNewNode->AddChild(std::move(leftPoint));
//    pNewNode->AddChild(std::move(rightPoint));
//    pNewNode->AddChild(std::move(upPoint));
//    pNewNode->AddChild(std::move(downPoint));
//    parentNode->AddChild(std::move(pNewNodeOwner));
//    return pNewNode;
//}