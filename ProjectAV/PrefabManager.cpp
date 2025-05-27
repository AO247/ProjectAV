#include "PrefabManager.h"
#include "Components.h"
#include "PhysicsCommon.h"

PrefabManager::~PrefabManager()
{
}


Node* PrefabManager::InstantiateColumn(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "WALL");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
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

Node* PrefabManager::InstantiateWall(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\sciany_1\\sciana_1.obj")
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

Node* PrefabManager::InstantiateStone1(Node* parentNode, float locX, float locY, float locZ, float scale) const {
    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enviro_male\\grzyb_1.obj")
    );

    BodyCreationSettings BodySettings(new JPH::BoxShape(Vec3(1.0f, 1.0f, 1.0f)), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
    BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    BodySettings.mMassPropertiesOverride.mMass = 7.0f;
    BodySettings.mFriction = 0.5f;
    //BodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;


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
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\platformy_glazy\\platforma_1.obj")
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

Node* PrefabManager::InstantiateIslandBig1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "GROUND");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspy\\wyspa_mala.obj")
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

	leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-24.0f, 0.0f, 5.0f));
	rightPoint->SetLocalPosition(DirectX::XMFLOAT3(24.0f, 0.0f, -10.0f));
	upPoint->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 24.0f));
	downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -24.0f));
    //pIsland->halfExtents = { 24.0f, 2.0f, 24.0f };
    //pIsland->halfExtents = { 10.0f, 2.0f, 10.0f };

	pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x)/2.0f - 1.0f,
							2.0f,
							(upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z)/2.0f - 1.0f };
    upPoint->AddComponent(
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
    );
    pIsland->leftPoint = leftPoint.get();
	pIsland->rightPoint = rightPoint.get();
	pIsland->upPoint = upPoint.get();
	pIsland->downPoint = downPoint.get();
    pNewNodeOwner->SetLocalPosition(DirectX::XMFLOAT3(locX, locY, locZ));
    pNewNodeOwner->SetLocalScale(DirectX::XMFLOAT3(scale, scale, scale));
    
    Node* pNewNode = pNewNodeOwner.get();

	
    Node* stone = InstantiateStone1(pNewNode, 5.2f, 30.0f, -5.2f, 1.0f);
	//Node* column1 = InstantiateColumn(pNewNode, 5.0f, 0.0f, -5.0f, 1.0f);
	//Node* column2 = InstantiateColumn(pNewNode, -5.0f, 0.0f, 5.0f, 1.0f);
	//Node* wall1 = InstantiateWall(pNewNode, 0.0f, 0.0f, -3.0f, 1.0f);
    Node* platform1 = InstantiatePlatform1(pNewNode, 0.0f, 0.0f, 0.0f, 2.0f);


    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    auto spawnPoint2 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
    auto spawnPoint3 = std::make_unique<Node>("SpawnPoint 2", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 2.0f, -3.0f));
    spawnPoint2->SetLocalPosition(DirectX::XMFLOAT3(-3.0f, 2.0f, 3.0f));
	spawnPoint3->SetLocalPosition(DirectX::XMFLOAT3(8.0f, 2.0f, -8.0f));
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
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_wycieta.obj")
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

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 11.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -11.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-9.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(11.0f, 0.0f, -4.0f));
    //pIsland->halfExtents = { 18.0f, 2.0f, 17.0f };
    pIsland->halfExtents = { (rightPoint->GetLocalPosition().x - leftPoint->GetLocalPosition().x)/2.0f - 1.0f,
                            2.0f,
                            (upPoint->GetLocalPosition().z - downPoint->GetLocalPosition().z)/2.0f - 1.0f };
   /* upPoint->AddComponent(
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
    Node* column1 = InstantiateColumn(pNewNode, -2.5f, 0.0f, 4.0f, 0.65f);
    //Node* column2 = InstantiateColumn(pNewNode, pNewNode->GetWorldPosition().x - 5.0f, pNewNode->GetWorldPosition().y + 0.0f, pNewNode->GetWorldPosition().z - 5.0f, 1.0f);
    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 2.0f, -3.0f));
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
    Node* column1 = InstantiateColumn(pNewNode, 5.0f, 0.0f, -5.0f, 1.0f);
    Node* column2 = InstantiateColumn(pNewNode, -5.0f, 0.0f, 5.0f, 1.0f);

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
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\objects\\wyspa_mala_1.obj")
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

    upPoint->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 0.0f, 11.0f));
    downPoint->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, -11.0f));
    leftPoint->SetLocalPosition(DirectX::XMFLOAT3(-9.0f, 0.0f, 5.0f));
    rightPoint->SetLocalPosition(DirectX::XMFLOAT3(11.0f, 0.0f, -4.0f));
    //pIsland->halfExtents = { 11.0f, 2.0f, 11.0f };
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
    Node* column1 = InstantiateColumn(pNewNode, 3.0f, 0.0f, 0.5f, 0.85f);
    //Node* column8 = InstantiateColumn(pNewNode, pNewNode->GetWorldPosition().x - 5.0f, pNewNode->GetWorldPosition().y + 0.0f, pNewNode->GetWorldPosition().z - 5.0f, 1.0f);

    auto spawnPoint1 = std::make_unique<Node>("SpawnPoint 1", pNewNodeOwner.get());
    spawnPoint1->SetLocalPosition(DirectX::XMFLOAT3(3.0f, 2.0f, -3.0f));
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
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\enemy\\basic.obj")
    );
    BodyCreationSettings eBodySettings(new JPH::CapsuleShape(1.0f, 1.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
    eBodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
    eBodySettings.mMassPropertiesOverride.mMass = 10.0f;
    eBodySettings.mFriction = 0.2f;
    eBodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
	eBodySettings.mMotionQuality = EMotionQuality::LinearCast;
    pNewNodeOwner->AddComponent(
        std::make_unique<Rigidbody>(pNewNodeOwner.get(), eBodySettings)
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