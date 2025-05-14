#include "App.h"
#include "Node.h"
#include "State.h"
#include "CMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include <memory>
#include <algorithm>
#include "CUtil.h"
#include "ColliderSphere.h"
#include "TexturePreprocessor.h"
#include "SolidCapsule.h"
#include "DebugLine.h"
#include "Testing.h"
#include "Button.h" // [NEW] Include the Button header

namespace dx = DirectX;


App::App(const std::string& commandLine)
    :
    commandLine(commandLine),
    wnd(1280, 720, "Project AV", this), // Pass window dimensions/title
    scriptCommander(TokenizeQuoted(commandLine)),
    pointLight(wnd.Gfx()), // Initialize PointLight
    pSceneRoot(std::make_unique<Node>("Root"))
{
    TestDynamicConstant();
    // Set Projection Matrix (Far plane adjusted for larger scenes potentially)
    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 2000.0f));

    // Initialize Physics Engine
    physicsEngine = PhysicsEngine();
    Raycast::physicsEngine = &physicsEngine; // Set the physics engine for raycasting
    // --- Create Nodes ---

    auto pCameraNodeOwner = std::make_unique<Node>("Camera", nullptr, "Camera");
    pCamera = pCameraNodeOwner.get();
    auto pFreeViewCameraOwner = std::make_unique<Node>("FreeViewCamera");
    pFreeViewCamera = pFreeViewCameraOwner.get();
    auto pPlayerOwner = std::make_unique<Node>("Player", nullptr, "Player");
    pPlayer = pPlayerOwner.get();
    auto pAbility1Owner = std::make_unique<Node>("Ability1", nullptr, "Ability1");
    pAbility1 = pAbility1Owner.get();
    auto pAbility2Owner = std::make_unique<Node>("Ability2", nullptr, "Ability2");
    pAbility2 = pAbility2Owner.get();
    auto pNanosuitOwner = std::make_unique<Node>("Nanosuit");
    pNanosuitNode = pNanosuitOwner.get();
    auto pEmptyNode = std::make_unique<Node>("EmptyNode");
    auto pBrickOwner = std::make_unique<Node>("Brick");
    pBrick = pBrickOwner.get();
    auto pBoxOwner = std::make_unique<Node>("Box", nullptr, "Wall");
    pBox = pBoxOwner.get();
    auto pStoneOwner = std::make_unique<Node>("Stone", nullptr, "Stone");
    pStone = pStoneOwner.get();
    auto pColumnOwner = std::make_unique<Node>("Column", nullptr, "Wall");
    pColumn = pColumnOwner.get();
    auto pColumn2Owner = std::make_unique<Node>("Column2", nullptr, "Wall");
    pColumn2 = pColumn2Owner.get();
    auto pColumn3Owner = std::make_unique<Node>("Column3", nullptr, "Wall");
    pColumn3 = pColumn3Owner.get();
    auto pColumn4Owner = std::make_unique<Node>("Column4", nullptr, "Wall");
    pColumn4 = pColumn4Owner.get();
    auto pIslandOwner = std::make_unique<Node>("Island", nullptr, "Ground");
    pIsland = pIslandOwner.get();
    auto pNoxTurnOwner = std::make_unique<Node>("NoxTurn");
    pNoxTurn = pNoxTurnOwner.get();
    auto pNoxTurnHairOwner = std::make_unique<Node>("NoxTurnHair");
    pNoxTurnHair = pNoxTurnHairOwner.get();
    auto pEnemyOwner = std::make_unique<Node>("Enemy", nullptr, "Enemy");
    pEnemy = pEnemyOwner.get();

    // Adding to Scene Graph
    pSceneRoot->AddChild(std::move(pCameraNodeOwner));
    pSceneRoot->AddChild(std::move(pFreeViewCameraOwner));
    pSceneRoot->AddChild(std::move(pPlayerOwner));
    pSceneRoot->AddChild(std::move(pAbility1Owner));
    pSceneRoot->AddChild(std::move(pAbility2Owner));
    //pSceneRoot->AddChild(std::move(pNanosuitOwner));
    //pSceneRoot->AddChild(std::move(pEmptyNode));
    //pSceneRoot->AddChild(std::move(pBrickOwner));
    pSceneRoot->AddChild(std::move(pBoxOwner));
    pSceneRoot->AddChild(std::move(pStoneOwner));
    pSceneRoot->AddChild(std::move(pColumnOwner));
    //pSceneRoot->AddChild(std::move(pColumn2Owner));
    //pSceneRoot->AddChild(std::move(pColumn3Owner));
    //pSceneRoot->AddChild(std::move(pColumn4Owner));
    pSceneRoot->AddChild(std::move(pIslandOwner));
    //pSceneRoot->AddChild(std::move(pNoxTurnOwner));
    //pNoxTurn->AddChild(std::move(pNoxTurnHairOwner));
    pSceneRoot->AddChild(std::move(pEnemyOwner));


    // Adding Models
    pNanosuitNode->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );
    pBrick->AddComponent(
        std::make_unique<ModelComponent>(pBrick, wnd.Gfx(), "Models\\brick_wall\\brick_wall.obj")
    );
    pBox->AddComponent(
        std::make_unique<ModelComponent>(pBox, wnd.Gfx(), "Models\\box.glb")
    );
    pStone->AddComponent(
        std::make_unique<ModelComponent>(pStone, wnd.Gfx(), "Models\\kamien\\kamien_6.obj")
    );
    //pStone->AddComponent(
    //    std::make_unique<ModelComponent>(pStone, wnd.Gfx(), "Models\\stone\\stone.glb")
    //);
    pColumn->AddComponent(
        std::make_unique<ModelComponent>(pColumn, wnd.Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    pColumn2->AddComponent(
        std::make_unique<ModelComponent>(pColumn2, wnd.Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    pColumn3->AddComponent(
        std::make_unique<ModelComponent>(pColumn3, wnd.Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    pColumn4->AddComponent(
        std::make_unique<ModelComponent>(pColumn4, wnd.Gfx(), "Models\\kolumna\\kolumna.obj")
    );

    pIsland->AddComponent(
        std::make_unique<ModelComponent>(pIsland, wnd.Gfx(), "Models\\wyspa\\wyspa_test.fbx")
    );


    pNoxTurn->AddComponent(
        std::make_unique<ModelComponent>(pNoxTurn, wnd.Gfx(), "Models\\stone\\char.fbx")
    );
    pNoxTurnHair->AddComponent(
        std::make_unique<ModelComponent>(pNoxTurnHair, wnd.Gfx(), "Models\\stone\\hair.fbx")
    );
    pEnemy->AddComponent(
        std::make_unique<ModelComponent>(pEnemy, wnd.Gfx(), "Models\\enemy\\basic.obj")
    );



    //Adding Rigidbody and Collider
    pPlayer->AddComponent(
        std::make_unique<Rigidbody>(pPlayer, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* pRigidbody = pPlayer->GetComponent<Rigidbody>();
    pPlayer->AddComponent(
        std::make_unique<PlayerController>(pPlayer, wnd) // Add controller first
    );
    pPlayer->AddComponent(
        std::make_unique<CapsuleCollider>(pPlayer, pRigidbody, 1.0f, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, pPlayer->GetComponent<PlayerController>()->height, 0.0f))
    );
    CapsuleCollider* pCapsule = pPlayer->GetComponent<CapsuleCollider>();
    pCapsule->SetLayer(Layers::PLAYER);
    pRigidbody->SetCollider(pCapsule);
    pRigidbody->SetMass(10.0f);
    physicsEngine.AddRigidbody(pRigidbody);

    pAbility1->AddComponent(
        std::make_unique<CapsuleCollider>(pAbility1, nullptr, 1.0f, Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 1.5f, 8.0f))
    );
    CapsuleCollider* a1CapsuleCollider = pAbility1->GetComponent<CapsuleCollider>();
    a1CapsuleCollider->SetIsTrigger(true);
    a1CapsuleCollider->SetTriggerEnabled(true);
    a1CapsuleCollider->SetLayer(Layers::PLAYER);
    physicsEngine.AddCollider(a1CapsuleCollider);

    pAbility2->AddComponent(
        std::make_unique<BoundingSphere>(pAbility2, Vector3(0.0f, 0.0f, 0.0f), 2.0f, nullptr)
    );
    BoundingSphere* a2Sphere = pAbility2->GetComponent<BoundingSphere>();
    a2Sphere->SetIsTrigger(true);
    a2Sphere->SetTriggerEnabled(true);
    a2Sphere->SetLayer(Layers::PLAYER);
    physicsEngine.AddCollider(a2Sphere);



    pEnemy->AddComponent(
        std::make_unique<Rigidbody>(pEnemy, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* eRigidbody = pEnemy->GetComponent<Rigidbody>();
    pEnemy->AddComponent(
        std::make_unique<CapsuleCollider>(pEnemy, eRigidbody, 1.0f, Vector3(-0.8f, 0.0f, -0.4f), Vector3(-0.8f, 4.0f, -0.4f))
    );
    CapsuleCollider* eCapsule = pEnemy->GetComponent<CapsuleCollider>();
    eCapsule->SetLayer(Layers::ENEMY);
    eRigidbody->SetCollider(eCapsule);
    physicsEngine.AddRigidbody(eRigidbody);


    pIsland->AddComponent(
        std::make_unique<OBB>(pIsland, nullptr, Vector3(0.0f, -0.3f, 0.0f), Vector3(50.0f, 1.0f, 50.0f))
    );
    OBB* iOBB = pIsland->GetComponent<OBB>();
    iOBB->SetLayer(Layers::GROUND);
    physicsEngine.AddCollider(iOBB);



    pBox->AddComponent(
        std::make_unique<BoundingSphere>(pBox, Vector3(0.0f, 0.0f, 0.0f), 2.0f, nullptr)
    );
    BoundingSphere* bBoundingSphere = pBox->GetComponent<BoundingSphere>();
    physicsEngine.AddCollider(bBoundingSphere);


    pColumn->AddComponent(
        std::make_unique<OBB>(pColumn, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
    );
    OBB* cOBB = pColumn->GetComponent<OBB>();
    physicsEngine.AddCollider(cOBB);
    //pColumn2->AddComponent(
    //	std::make_unique<OBB>(pColumn2, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
    //);
    //OBB* cOBB2 = pColumn2->GetComponent<OBB>();
    //physicsEngine.AddCollider(cOBB2);
    //pColumn3->AddComponent(
    //	std::make_unique<OBB>(pColumn3, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
    //);
    //OBB* cOBB3 = pColumn3->GetComponent<OBB>();
    //physicsEngine.AddCollider(cOBB3);
    //pColumn4->AddComponent(
    //	std::make_unique<OBB>(pColumn4, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
    //);
    //OBB* cOBB4 = pColumn4->GetComponent<OBB>();
    //physicsEngine.AddCollider(cOBB4);

    pStone->AddComponent(
        std::make_unique<Rigidbody>(pStone, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* sRigidbody = pStone->GetComponent<Rigidbody>();
    pStone->AddComponent(
        std::make_unique<Throwable>(pStone)
    );
    pStone->AddComponent(
        std::make_unique<OBB>(pStone, sRigidbody, Vector3(0.0f, 0.6f, 0.0f), Vector3(1.2f, 1.1f, 1.7f))
    );
    OBB* sOBB = pStone->GetComponent<OBB>();
    sRigidbody->SetCollider(sOBB);
    physicsEngine.AddRigidbody(sRigidbody);
    pStone->AddComponent(
        std::make_unique<BoundingSphere>(pStone, Vector3(0.0f, 0.6f, 0.0f), 1.5f, nullptr)
    );
    BoundingSphere* sBoundingSphere = pStone->GetComponent<BoundingSphere>();
    sBoundingSphere->SetIsTrigger(true);
    sBoundingSphere->SetTriggerEnabled(true);
    pStone->GetComponent<Throwable>()->damageArea = sBoundingSphere;
    physicsEngine.AddCollider(sBoundingSphere);

    //Adding Other Components
    pFreeViewCamera->AddComponent(
        std::make_unique<Camera>(pFreeViewCamera, wnd)
    );
    pCamera->AddComponent(
        std::make_unique<Camera>(pCamera, wnd)
    );
    pCamera->GetComponent<Camera>()->active = true;


    pEnemy->AddComponent(
        std::make_unique<StateMachine>(pEnemy, StateType::IDLE)
    );
    pEnemy->GetComponent<StateMachine>()->AddComponent(
        std::make_unique<Walking>(pEnemy)
    );
    pEnemy->GetComponent<StateMachine>()->UpdateComponents();

    
    // Changing position scale etc.]
    pFreeViewCamera->SetLocalPosition({ 4.0f, 11.0f, -28.0f });
    pPlayer->SetLocalPosition({ 0.0f, 35.0f, 0.0f });
    pBox->SetLocalPosition(DirectX::XMFLOAT3(-10.0f, 3.0f, 10.0f));
    pBrick->SetLocalScale(dx::XMFLOAT3(20.0f, 20.0f, 1.0f));
    pBrick->SetLocalRotation(dx::XMFLOAT3(DirectX::XMConvertToRadians(90), 0.0f, 0.0f));
    pStone->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 1.0f, 10.0f));
    pStone->SetLocalScale(dx::XMFLOAT3(1.5f, 1.5f, 1.5f));
    pIsland->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
    pIsland->SetLocalScale(dx::XMFLOAT3(1.3f, 1.3f, 1.3f));
    pNoxTurn->SetLocalPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f));
    pNoxTurn->SetLocalScale(dx::XMFLOAT3(0.01f, 0.01f, 0.01f));
    pEnemy->SetLocalPosition(DirectX::XMFLOAT3(15.0f, 10.0f, 0.0f));
    pColumn->SetLocalPosition(DirectX::XMFLOAT3(-8.0f, 0.0f, -7.0f));
    pColumn2->SetLocalPosition(DirectX::XMFLOAT3(-2.0f, 0.0f, 4.0f));
    pColumn3->SetLocalPosition(DirectX::XMFLOAT3(-16.0f, 0.0f, -6.0f));
    pColumn4->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 0.0f, 6.0f));

    //Adding colliders to draw
    AddBoxColliderToDraw(wnd.Gfx(), iOBB);
    AddBoxColliderToDraw(wnd.Gfx(), sOBB);
    AddBoxColliderToDraw(wnd.Gfx(), cOBB);
    /*AddBoxColliderToDraw(wnd.Gfx(), cOBB2);
    AddBoxColliderToDraw(wnd.Gfx(), cOBB3);
    AddBoxColliderToDraw(wnd.Gfx(), cOBB4);*/


    AddSphereColliderToDraw(wnd.Gfx(), bBoundingSphere);
    AddSphereColliderToDraw(wnd.Gfx(), a2Sphere);
    AddSphereColliderToDraw(wnd.Gfx(), sBoundingSphere);

    AddCapsuleColliderToDraw(wnd.Gfx(), pCapsule);
    AddCapsuleColliderToDraw(wnd.Gfx(), eCapsule);
    AddCapsuleColliderToDraw(wnd.Gfx(), a1CapsuleCollider);

    this->myButton = std::make_unique<Button>(wnd.Gfx().GetDevice(), 110, 110, 200, 100, L"Click Me");



    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
    cursorEnabled = false;
}

App::~App() = default;

int App::Go()
{
    float lag = 0.0f;
    const float FIXED_TIME_STEP = 0.01666f;
    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        const auto dt = timer.Mark() * speed_factor;
        lag += dt;

        while (lag >= FIXED_TIME_STEP)
        {
            physicsEngine.Simulate(FIXED_TIME_STEP);
            lag -= FIXED_TIME_STEP;
        }
        physicsEngine.Simulate(lag);
        lag = 0.0f;

        HandleInput(dt);
        DoFrame(dt);
    }
}

// --- UPDATED HandleInput ---
void App::HandleInput(float dt)
{
    // --- Only handle non-player input here ---
    while (const auto e = wnd.kbd.ReadKey())
    {
        if (!e->IsPress()) continue;

        switch (e->GetCode())
        {
        case 'C': // Toggle cursor
            if (cursorEnabled) {
                wnd.DisableCursor();
                wnd.mouse.EnableRaw();
            }
            else {
                wnd.EnableCursor();
                wnd.mouse.DisableRaw();
            }
            cursorEnabled = !cursorEnabled;
            break;
        case 'H': // Toggle UI
            showControlWindow = !showControlWindow;
            break;
        case VK_ESCAPE: // Exit
            PostQuitMessage(0);
            return;
        case VK_F1: // Toggle ImGui Demo
            showDemoWindow = !showDemoWindow;
            break;
        case 'V':
        {
            freeViewCamera = !freeViewCamera;
            if (freeViewCamera) {
                pCamera->GetComponent<Camera>()->active = false;
                pFreeViewCamera->GetComponent<Camera>()->active = true;
            }
            else {
                pCamera->GetComponent<Camera>()->active = true;
                pFreeViewCamera->GetComponent<Camera>()->active = false;
            }
            break;
        }
        }
    }

    // FreeCamera Movement
    if (freeViewCamera) {
        if (wnd.kbd.KeyIsPressed('I')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.0f, 0.1f });
        }
        if (wnd.kbd.KeyIsPressed('K')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.0f, -0.1f });
        }
        if (wnd.kbd.KeyIsPressed('J')) {
            pFreeViewCamera->TranslateLocal({ -0.1f, 0.0f, 0.0f });
        }
        if (wnd.kbd.KeyIsPressed('L')) {
            pFreeViewCamera->TranslateLocal({ 0.1f, 0.0f, 0.0f });
        }
        if (wnd.kbd.KeyIsPressed('U')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, -0.1f, 0.0f });
        }
        if (wnd.kbd.KeyIsPressed('O')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.1f, 0.0f });
        }
    }

}

void App::DoFrame(float dt)
{
    pSceneRoot->Update(dt);

    wnd.Gfx().BeginFrame(0.5f, 0.5f, 1.0f);
    //if (pPlayer->GetLocalPosition().y < -10.0f) {
    //	pPlayer->SetLocalPosition({ -20.0f, 225.0f, -25.0f });
    //    pEnemy->SetLocalPosition({ 15.0f, 225.0f, 0.0f });
    //}
    dx::XMMATRIX viewMatrix = pCamera->GetComponent<Camera>()->GetViewMatrix();
    if (freeViewCamera)
    {
        viewMatrix = pFreeViewCamera->GetComponent<Camera>()->GetViewMatrix();
    }
    wnd.Gfx().SetCamera(viewMatrix);

    // --- Bind Lights ---
    pointLight.Bind(wnd.Gfx(), viewMatrix); // Bind point light (to slot 0)

    //FrustumCalculating(); // Draw with FRUSTUM CULLING
    //pSceneRoot->Draw(wnd.Gfx()); // Draw without FRUSTUM CULLING you have to also uncomment the draw method in Node.cpp
    //FrameCommander fc; // Create FrameCommander for this frame

    //// Submit the entire scene graph to the FrameCommander
    //// Pass Graphics context if Submit methods require it
    pSceneRoot->Submit(fc, wnd.Gfx());

    //// Also submit the point light's mesh to a pass (e.g., main pass 0)
    //// Execute all rendering passes accumulated in the FrameCommander
    //Vector3 previousRotation = pEnemy->GetLocalRotationEuler();

 //   Vector3 targetPosition = pPlayer->GetWorldPosition();
    //Rigidbody* rigidbody = pEnemy->GetComponent<Rigidbody>();

 //   Vector3 temporaryDirection = targetPosition - rigidbody->GetPosition();
 //   temporaryDirection.Normalize();
 //   /*float targetYaw = atan2f(temporaryDirection.x, temporaryDirection.z);
 //   pEnemy->SetLocalRotation({ 0.0f, targetYaw, 0.0f });*/
 //   float radius = pEnemy->GetComponent<CapsuleCollider>()->GetRadius();

 //   Vector3 pos = pEnemy->GetComponent<CapsuleCollider>()->GetTransformedBase();
 //   Vector3 forward = pEnemy->Forward();
 //   Vector3 right = pEnemy->Right();

 //   Vector3 centerOrigin = pos + forward;
 //   Vector3 leftOrigin = centerOrigin - right * pEnemy->GetComponent<CapsuleCollider>()->GetRadius();
 //   Vector3 rightOrigin = centerOrigin + right * pEnemy->GetComponent<CapsuleCollider>()->GetRadius();

 //   Vector3 centerDir = forward;

 //   // Vector3 leftDir = (forward - right * 0.3f); leftDir.Normalize(); 
 //   // Vector3 rightDir = (forward + right * 0.3f); rightDir.Normalize();

 //   RaycastData hitLeft = Raycast::CastThroughLayers(leftOrigin, temporaryDirection, std::vector<Layers>{ENEMY});
 //   DebugLine(wnd.Gfx(), leftOrigin, hitLeft.hitPoint).Draw(wnd.Gfx());

 //   RaycastData hitRight = Raycast::CastThroughLayers(rightOrigin, temporaryDirection, std::vector<Layers>{ENEMY});
 //   DebugLine(wnd.Gfx(), rightOrigin, hitRight.hitPoint).Draw(wnd.Gfx());

 //   RaycastData hitCenter = Raycast::CastThroughLayers(centerOrigin, temporaryDirection, std::vector<Layers>{ENEMY});
 //   DebugLine(wnd.Gfx(), centerOrigin, hitCenter.hitPoint).Draw(wnd.Gfx());

    //pEnemy->SetLocalRotation(previousRotation);
    //ector3 secPos(pCamera->GetWorldPosition().x + pCamera->Forward().x, pCamera->GetWorldPosition().y + pCamera->Forward().y, pCamera->GetWorldPosition().z + pCamera->Forward().z);
    //DebugLine(wnd.Gfx(), pCamera->GetWorldPosition(), secPos).Draw(wnd.Gfx());

    
    if (showControlWindow) {
        ShowControlWindows();
    }
  
    fc.Execute(wnd.Gfx());
    myButton->Draw(wnd.Gfx().GetContext());
    wnd.Gfx().EndFrame();
    fc.Reset();
}


void App::FrustumCalculating() {
    dx::XMMATRIX camWorldTransform = pCamera->GetWorldTransform();
    dx::XMVECTOR camWorldPos = camWorldTransform.r[3]; // Default
    dx::XMVECTOR camWorldForward = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), camWorldTransform)); // Default
    dx::XMVECTOR camWorldUp = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), camWorldTransform));    // Default
    dx::XMVECTOR camWorldRight = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), camWorldTransform)); // Default

    const float fovAngleY = DirectX::XMConvertToRadians(70.0f); // Field of View in Y direction (radians) - MUST MATCH YOUR PROJECTION
    const float aspectRatio = 16.0f / 9.0f; // MUST MATCH YOUR PROJECTION
    const float nearDist = 0.5f;         // MUST MATCH YOUR PROJECTION
    const float farDist = 300.0f;       // MUST MATCH YOUR PROJECTION

    // Half heights/widths at near and far planes
    float halfHeightNear = nearDist * tanf(fovAngleY * 0.5f);
    float halfWidthNear = halfHeightNear * aspectRatio;
    float halfHeightFar = farDist * tanf(fovAngleY * 0.5f);
    float halfWidthFar = halfHeightFar * aspectRatio;

    dx::XMVECTOR nearCenter = dx::XMVectorAdd(camWorldPos, dx::XMVectorScale(camWorldForward, nearDist));
    dx::XMVECTOR farCenter = dx::XMVectorAdd(camWorldPos, dx::XMVectorScale(camWorldForward, farDist));

    // Calculate the 8 corner points (optional for plane calculation, but useful for verification)
    dx::XMVECTOR nearTopLeft = dx::XMVectorAdd(nearCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearTopRight = dx::XMVectorAdd(nearCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearBottomLeft = dx::XMVectorAdd(nearCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, -halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearBottomRight = dx::XMVectorAdd(nearCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, -halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR farTopLeft = dx::XMVectorAdd(farCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farTopRight = dx::XMVectorAdd(farCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farBottomLeft = dx::XMVectorAdd(farCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, -halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farBottomRight = dx::XMVectorAdd(farCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, -halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));

    cameraFrustum.Near = 0.5f;

    // Far Plane: Normal = -camWorldForward, Point = farCenter
    cameraFrustum.Far = 300.0f;

    float tanHalfFovY = tanf(fovAngleY * 0.5f);
    cameraFrustum.TopSlope = tanHalfFovY;
    cameraFrustum.BottomSlope = -tanHalfFovY;

    cameraFrustum.RightSlope = tanHalfFovY * aspectRatio;
    cameraFrustum.LeftSlope = -tanHalfFovY * aspectRatio;

    DirectX::XMStoreFloat3(&cameraFrustum.Origin, camWorldPos);
    dx::XMMATRIX worldOrientationMatrix;
    worldOrientationMatrix.r[0] = camWorldRight;   // World X-axis of the camera
    worldOrientationMatrix.r[1] = camWorldUp;      // World Y-axis of the camera
    worldOrientationMatrix.r[2] = camWorldForward; // World Z-axis of the camera
    worldOrientationMatrix.r[3] = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Set W component for a valid matrix

    // 3. Convert the Orientation Matrix to a Quaternion
    DirectX::XMStoreFloat4(&cameraFrustum.Orientation, dx::XMQuaternionRotationMatrix(worldOrientationMatrix));


    DrawNodeRecursive(wnd.Gfx(), *pSceneRoot);
}

void App::DrawNodeRecursive(Graphics& gfx, Node& node)
{
    // --- Culling Check ---
    bool shouldDraw = true; // Assume we draw by default
    ModelComponent* modelComp = node.GetComponent<ModelComponent>();

    if (modelComp != nullptr) // Only cull nodes with models (or add BoundsComponent later)
    {
        DirectX::BoundingSphere sphere;
        DirectX::BoundingBox box;
        DirectX::ContainmentType containment = DirectX::DISJOINT;
        if (node.GetComponent<BoundingSphere>() != nullptr)
        {
            sphere.Center = node.GetWorldPosition();
            sphere.Radius = node.GetComponent<BoundingSphere>()->GetRadius();
            containment = cameraFrustum.Contains(sphere);
        }
        else if (node.GetComponent<OBB>() != nullptr)
        {
            box.Center = node.GetWorldPosition();
            box.Extents = node.GetComponent<OBB>()->GetTransformedSize();
            containment = cameraFrustum.Contains(box);
        }
        else if (node.GetComponent<CapsuleCollider>() != nullptr)
        {
            // Assuming you have a method to get the capsule's bounding sphere
            sphere.Center = DirectX::XMFLOAT3(node.GetWorldPosition().x,
                (node.GetWorldPosition().y + 1.5f),
                node.GetWorldPosition().z);
            sphere.Radius = node.GetComponent<CapsuleCollider>()->GetRadius() * 2.5f;
            containment = cameraFrustum.Contains(sphere);
        }

        if (containment == DirectX::DISJOINT) // DISJOINT means completely outside
        {
            shouldDraw = false; // Don't draw this node or its children
        }

    }


    if (shouldDraw)
    {
        //node.Draw(gfx);
        for (const auto& pChild : node.GetChildren())
        {
            if (pChild)
            {
                DrawNodeRecursive(gfx, *pChild); // Recurse for children
            }
        }
    }
}

void App::ShowControlWindows()
{
    // --- Existing Windows ---

    DrawSphereColliders(wnd.Gfx()); // Call the updated function
    DrawBoxColliders(wnd.Gfx()); // Call the updated function
    DrawCapsuleColliders(wnd.Gfx());
    pointLight.Submit(fc);

    pointLight.SpawnControlWindow(); // Control for Point Light
    if (showDemoWindow)
    {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    // --- Show Model Component Windows ---
    //if (pNanosuitNode)
    //{
    //    if (auto* modelComp = pNanosuitNode->GetComponent<ModelComponent>())
    //    {
    //        modelComp->ShowWindow("Nanosuit Controls");
    //    }
    //}

    // --- Simulation Speed Window ---
    if (ImGui::Begin("Simulation Speed"))
    {
        ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f);
        ImGui::Text("To change camera press 'C'");
        ImGui::Text("To show/hide control window press 'H'");
    }
    ImGui::End();


    // --- NEW: Scene Hierarchy Window ---
    if (ImGui::Begin("Scene Hierarchy"))
    {
        if (pSceneRoot)
        {
            pSceneRoot->ShowNodeTree(pSelectedSceneNode);
        }

        ImGui::Separator();

        if (pSelectedSceneNode != nullptr)
        {
            ImGui::Text("Selected: %s", pSelectedSceneNode->GetName().c_str());
            ImGui::Separator();

            // --- Transform Editor ---
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Get current values
                DirectX::XMFLOAT3 pos = pSelectedSceneNode->GetLocalPosition();
                DirectX::XMFLOAT3 rotRad = pSelectedSceneNode->GetLocalRotationEuler();
                DirectX::XMFLOAT3 scale = pSelectedSceneNode->GetLocalScale();

                // Convert rotation to degrees for easier editing
                DirectX::XMFLOAT3 rotDeg = {
                    DirectX::XMConvertToDegrees(rotRad.x),
                    DirectX::XMConvertToDegrees(rotRad.y),
                    DirectX::XMConvertToDegrees(rotRad.z)
                };

                bool transformChanged = false;

                ImGui::Text("Position"); ImGui::SameLine();
                if (ImGui::DragFloat3("##Position", &pos.x, 0.1f))
                {
                    pSelectedSceneNode->SetLocalPosition(pos);
                    transformChanged = true;
                }

                ImGui::Text("Rotation"); ImGui::SameLine();
                if (ImGui::DragFloat3("##Rotation", &rotDeg.x, 1.0f)) // Edit degrees
                {
                    // Convert back to radians before setting
                    rotRad = {
                        DirectX::XMConvertToRadians(rotDeg.x),
                        DirectX::XMConvertToRadians(rotDeg.y),
                        DirectX::XMConvertToRadians(rotDeg.z)
                    };
                    pSelectedSceneNode->SetLocalRotation(rotRad); // Set radians
                    transformChanged = true;
                }

                ImGui::Text("Scale   "); ImGui::SameLine(); // Extra spaces for alignment
                if (ImGui::DragFloat3("##Scale", &scale.x, 0.01f, 0.01f, 100.0f)) // Add min/max scale
                {
                    // Prevent zero scale if needed
                    scale.x = std::max(scale.x, 0.001f);
                    scale.y = std::max(scale.y, 0.001f);
                    scale.z = std::max(scale.z, 0.001f);
                    pSelectedSceneNode->SetLocalScale(scale);
                    transformChanged = true;
                }

                // Note: The Node's worldTransformDirty flag is already set by the SetLocal... methods.
                // The Node::Update() call later will handle recalculating the world matrix.
            }

            // --- Component Viewer ---
            if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
            {
                const auto& components = pSelectedSceneNode->GetComponents();
                if (components.empty()) {
                    ImGui::TextDisabled("No components attached.");
                }
                else {
                    int compIndex = 0;
                    for (const auto& comp : components) {
                        if (comp) {
                            // Create a unique ID for the component header
                            std::string compLabel = typeid(*comp).name();
                            // Remove "class " prefix if present (platform dependent)
                            if (compLabel.rfind("class ", 0) == 0) {
                                compLabel = compLabel.substr(6);
                            }
                            compLabel += "##" + std::to_string(compIndex++); // Add unique ID

                            // Make each component collapsible
                            if (ImGui::TreeNode(compLabel.c_str()))
                            {
                                // --- Call the component's ImGui draw function ---
                                comp->DrawImGuiControls();
                                // --- End Call ---

                                ImGui::TreePop();
                            }
                        }
                    }
                }
            }

        }
        else
        {
            ImGui::Text("Selected: None");
        }
    }
    ImGui::End(); // End Scene Hierarchy Window
}
void App::AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere)
{
    // Using default constructor then initialize might be slightly cleaner if SolidSphere allows it
    ColliderSphere* sphereCollider = new ColliderSphere(gfx, boundingSphere->GetRadius());
    sphereCollidersToDraw[boundingSphere] = sphereCollider;

}

void App::DrawSphereColliders(Graphics& gfx)
{
    for (auto it = sphereCollidersToDraw.begin(); it != sphereCollidersToDraw.end(); ++it)
    {
        it->second->SetPos(DirectX::XMFLOAT3(it->first->GetTransformedCenter().x,
            it->first->GetTransformedCenter().y,
            it->first->GetTransformedCenter().z));
        it->second->Submit(fc);
    }
}



// Signature now takes OBB*
void App::AddBoxColliderToDraw(Graphics& gfx, OBB* obb)
{
    SolidBox* box = new SolidBox(gfx, obb->GetTransformedCenter(), obb->GetTransformedSize());
    boxCollidersToDraw[obb] = box;

}

// Logic now uses Rigidbody's world transform for OBB
void App::DrawBoxColliders(Graphics& gfx)
{
    for (auto it = boxCollidersToDraw.begin(); it != boxCollidersToDraw.end(); ++it)
    {
        it->second->SetPos(DirectX::XMFLOAT3(it->first->GetTransformedCenter().x,
            it->first->GetTransformedCenter().y,
            it->first->GetTransformedCenter().z));
        it->second->SetSize(DirectX::XMFLOAT3(it->first->GetTransformedSize().x,
            it->first->GetTransformedSize().y,
            it->first->GetTransformedSize().z));
        it->second->Submit(fc);

    }

}

void App::AddCapsuleColliderToDraw(Graphics& gfx, CapsuleCollider* capsule)
{
    SolidCapsule* solidCapsule = new SolidCapsule(gfx, capsule->GetTransformedBase(), capsule->GetTransformedTip(), capsule->GetRadius());
    capsuleCollidersToDraw[capsule] = solidCapsule;
}

void App::DrawCapsuleColliders(Graphics& gfx)
{
    for (auto it = capsuleCollidersToDraw.begin(); it != capsuleCollidersToDraw.end(); ++it)
    {
        it->second->SetBase(it->first->GetTransformedBase());
        it->second->SetTip(it->first->GetTransformedTip());
        it->second->SetRadius(it->first->GetRadius());
        it->second->Submit(fc);
    }
}

// [NEW] Handle mouse click event
void App::OnMouseClick(int xPos, int yPos) {
    if (myButton->IsClicked(xPos, yPos)) {
        // Button was clicked!
        OutputDebugStringW(L"Button Clicked!\n");
    }
}

