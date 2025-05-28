#include "PrefabManager.h"
#include "Components.h"
#include "PhysicsCommon.h"

PrefabManager::~PrefabManager()
{
}


Node* PrefabManager::InstantiateColumn(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Column", nullptr, "Wall");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    BodyCreationSettings BodySettings(new JPH::BoxShape(Vec3(2.0f, 10.0f, 2.0f)), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Static, Layers::WALL);
    BodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
    BodySettings.mMassPropertiesOverride.mMass = 50.0f;
    BodySettings.mFriction = 1.0f;
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

Node* PrefabManager::InstantiateStone(Node* parentNode, float locX, float locY, float locZ, float scale) const {
    auto pNewNodeOwner = std::make_unique<Node>("Stone", nullptr, "Stone");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\kamien\\kamien_6.obj")
    );

    BodyCreationSettings BodySettings(new JPH::BoxShape(Vec3(1.0f * scale, 1.0f * scale, 1.0f * scale)), RVec3(locX, locY, locZ), Quat::sIdentity(), EMotionType::Dynamic, Layers::WALL);
    BodySettings.mOverrideMassProperties = EOverrideMassProperties::CalculateInertia;
    BodySettings.mMassPropertiesOverride.mMass = 5.0f;
    BodySettings.mFriction = 0.5f;
    BodySettings.mMotionQuality = EMotionQuality::LinearCast;
    BodySettings.mAngularDamping = 10.0f;
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

Node* PrefabManager::InstantiateIslandBig1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island B1", nullptr, "Ground");

    pNewNodeOwner->AddComponent(
        std::make_unique<ModelComponent>(pNewNodeOwner.get(), wnd->Gfx(), "Models\\wyspa\\wyspa_test.fbx")
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

    Node* stone = InstantiateStone(pNewNode, 10.2f, 30.0f, -5.2f, 1.5f);
	Node* column1 = InstantiateColumn(pNewNode, 5.0f, 0.0f, -5.0f, 1.0f);
	Node* column2 = InstantiateColumn(pNewNode, -5.0f, 0.0f, 5.0f, 1.0f);

	pNewNode->AddChild(std::move(leftPoint));
	pNewNode->AddChild(std::move(rightPoint));
	pNewNode->AddChild(std::move(upPoint));
	pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateIslandMedium1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island M1", nullptr, "Ground");

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

    pNewNode->AddChild(std::move(leftPoint));
    pNewNode->AddChild(std::move(rightPoint));
    pNewNode->AddChild(std::move(upPoint));
    pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}


Node* PrefabManager::InstantiateIslandMedium2(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island M2", nullptr, "Ground");

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

    pNewNode->AddChild(std::move(leftPoint));
    pNewNode->AddChild(std::move(rightPoint));
    pNewNode->AddChild(std::move(upPoint));
    pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}

Node* PrefabManager::InstantiateIslandSmall1(Node* parentNode, float locX, float locY, float locZ, float scale) const {


    auto pNewNodeOwner = std::make_unique<Node>("Island S1", nullptr, "Ground");

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

    pNewNode->AddChild(std::move(leftPoint));
    pNewNode->AddChild(std::move(rightPoint));
    pNewNode->AddChild(std::move(upPoint));
    pNewNode->AddChild(std::move(downPoint));
    parentNode->AddChild(std::move(pNewNodeOwner));
    return pNewNode;
}