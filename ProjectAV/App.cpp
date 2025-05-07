#include "App.h"
#include "Node.h"           
#include "State.h"
#include "CMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <memory>
#include <shellapi.h>
#include <algorithm>
#include "ColliderSphere.h"
#include "TexturePreprocessor.h"
#include "SolidCapsule.h"
#include "SoundDevice.h"
#include "MusicBuffer.h"
#include "SoundEffectsLibrary.h"
#include "SoundEffectsPlayer.h"

namespace dx = DirectX;

GDIPlusManager gdipm;



App::App(const std::string& commandLine)
    :
    commandLine(commandLine),
    wnd(1280, 720, "Project AV"), // Pass window dimensions/title
    pointLight(wnd.Gfx()), // Initialize PointLight
    pSceneRoot(std::make_unique<Node>("Root"))
{
    // Set Projection Matrix (Far plane adjusted for larger scenes potentially)
    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 2000.0f));
    if (this->commandLine != "")
    {
        int nArgs;
        const auto pLineW = GetCommandLineW();
        const auto pArgs = CommandLineToArgvW(pLineW, &nArgs);
        if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-objnorm")
        {
            const std::wstring pathInWide = pArgs[2];
            TexturePreprocessor::FlipYAllNormalMapsInObj(
                std::string(pathInWide.begin(), pathInWide.end())
            );
            throw std::runtime_error("Normal maps all processed successfully. Just kidding about that whole runtime error thing.");
        }
        else if (nArgs >= 3 && std::wstring(pArgs[1]) == L"--twerk-flipy")
        {
            const std::wstring pathInWide = pArgs[2];
            const std::wstring pathOutWide = pArgs[3];
            TexturePreprocessor::FlipYNormalMap(
                std::string(pathInWide.begin(), pathInWide.end()),
                std::string(pathOutWide.begin(), pathOutWide.end())
            );
            throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
        }
        else if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--twerk-validate")
        {
            const std::wstring minWide = pArgs[2];
            const std::wstring maxWide = pArgs[3];
            const std::wstring pathWide = pArgs[4];
            TexturePreprocessor::ValidateNormalMap(
                std::string(pathWide.begin(), pathWide.end()), std::stof(minWide), std::stof(maxWide)
            );
            throw std::runtime_error("Normal map validated successfully. Just kidding about that whole runtime error thing.");
        }
    }
	// Initialize Physics Engine
	physicsEngine = PhysicsEngine();
	Raycast::physicsEngine = &physicsEngine; // Set the physics engine for raycasting

	// --- Initialize Sound System ---
    SoundDevice::Init();
    //static SoundEffectsPlayer effectsPlayer1;
    uint32_t sound1 = SE_LOAD("D:\\GameDev\\ProjectAV\\ProjectAV\\Models\\turn.ogg");
    myMusic = std::make_unique<MusicBuffer>("D:\\GameDev\\ProjectAV\\ProjectAV\\Models\\muza_full.wav");
    
    

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
	pSceneRoot ->AddChild(std::move(pAbility1Owner));
	pSceneRoot->AddChild(std::move(pAbility2Owner));
    //pSceneRoot->AddChild(std::move(pNanosuitOwner));
    //pSceneRoot->AddChild(std::move(pEmptyNode));
    //pSceneRoot->AddChild(std::move(pBrickOwner));
    pSceneRoot->AddChild(std::move(pBoxOwner));
    pSceneRoot->AddChild(std::move(pStoneOwner));
    pSceneRoot->AddChild(std::move(pColumnOwner));
	pSceneRoot->AddChild(std::move(pColumn2Owner));
	pSceneRoot->AddChild(std::move(pColumn3Owner));
	pSceneRoot->AddChild(std::move(pColumn4Owner));
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
		std::make_unique<ModelComponent>(pStone, wnd.Gfx(), "Models\\kamien\\kamien_test.fbx")
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
	physicsEngine.AddCollider(a1CapsuleCollider);

	pAbility2->AddComponent(
        std::make_unique<BoundingSphere>(pAbility2, Vector3(0.0f, 0.0f, 0.0f), 2.0f, nullptr)
    );
    BoundingSphere* a2Sphere = pAbility2->GetComponent<BoundingSphere>();
    a2Sphere->SetIsTrigger(true);
    a2Sphere->SetTriggerEnabled(true);
	physicsEngine.AddCollider(a2Sphere);
    


    pEnemy->AddComponent(
        std::make_unique<Rigidbody>(pEnemy, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* eRigidbody = pEnemy->GetComponent<Rigidbody>();
	pEnemy->AddComponent(
		std::make_unique<CapsuleCollider>(pEnemy, eRigidbody, 1.0f, Vector3(-0.8f, 0.0f, -0.4f), Vector3(-0.8f, 4.0f, -0.4f))
	);
	CapsuleCollider* eCapsule = pEnemy->GetComponent<CapsuleCollider>();
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
	pColumn2->AddComponent(
		std::make_unique<OBB>(pColumn2, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
	);
	OBB* cOBB2 = pColumn2->GetComponent<OBB>();
	physicsEngine.AddCollider(cOBB2);
	pColumn3->AddComponent(
		std::make_unique<OBB>(pColumn3, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
	);
	OBB* cOBB3 = pColumn3->GetComponent<OBB>();
	physicsEngine.AddCollider(cOBB3);
	pColumn4->AddComponent(
		std::make_unique<OBB>(pColumn4, nullptr, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
	);
	OBB* cOBB4 = pColumn4->GetComponent<OBB>();
	physicsEngine.AddCollider(cOBB4);
    
    pStone->AddComponent(
        std::make_unique<Rigidbody>(pStone, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
	Rigidbody* sRigidbody = pStone->GetComponent<Rigidbody>();
	pStone->AddComponent(
		std::make_unique<OBB>(pStone, sRigidbody, Vector3(0.0f, 0.6f, 0.0f), Vector3(1.2f, 1.1f, 1.7f))
	);
	OBB* sOBB = pStone->GetComponent<OBB>();
	sRigidbody->SetCollider(sOBB);
	physicsEngine.AddRigidbody(sRigidbody);

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
	AddBoxColliderToDraw(wnd.Gfx(), cOBB2);
	AddBoxColliderToDraw(wnd.Gfx(), cOBB3);
	AddBoxColliderToDraw(wnd.Gfx(), cOBB4);


	AddSphereColliderToDraw(wnd.Gfx(), bBoundingSphere);
	AddSphereColliderToDraw(wnd.Gfx(), a2Sphere);

	AddCapsuleColliderToDraw(wnd.Gfx(), pCapsule);
	AddCapsuleColliderToDraw(wnd.Gfx(), eCapsule);
	AddCapsuleColliderToDraw(wnd.Gfx(), a1CapsuleCollider);

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
		case 'M': // Toggle Music
            if (myMusic->isPlaying()) // toggle play/pause
            {
                myMusic->Pause();
            }
            else
            {
                myMusic->Play();
            }
            break;
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

    pSceneRoot->Draw(wnd.Gfx());

	if (myMusic->isPlaying())
	{
		myMusic->UpdateBufferStream();
	}


    if (showControlWindow) {
        ShowControlWindows();
    }

    wnd.Gfx().EndFrame();
}


void App::ShowControlWindows()
{
    // --- Existing Windows ---
    DrawSphereColliders(wnd.Gfx());
    DrawBoxColliders(wnd.Gfx()); // Call the updated function
	DrawCapsuleColliders(wnd.Gfx());
    pointLight.Draw(wnd.Gfx());

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
        ImGui::SliderFloat("Speed Factor", &speed_factor, 0.0f, 4.0f, "%.2f");
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

    sphereCollidersToDraw[boundingSphere] = SolidSphere(gfx, boundingSphere->GetRadius());
}

void App::DrawSphereColliders(Graphics& gfx)
{
    for (auto it = sphereCollidersToDraw.begin(); it != sphereCollidersToDraw.end(); ++it)
    {
        ColliderSphere sphere(gfx, it->first->GetRadius());
        sphere.SetPos(DirectX::XMFLOAT3(it->first->GetTransformedCenter().x,
            it->first->GetTransformedCenter().y,
            it->first->GetTransformedCenter().z));
        sphere.Draw(gfx);
    }
}



// Signature now takes OBB*
void App::AddBoxColliderToDraw(Graphics& gfx, OBB* obb)
{

    boxCollidersToDraw[obb] = SolidSphere(gfx, 10.0f);

}

// Logic now uses Rigidbody's world transform for OBB
void App::DrawBoxColliders(Graphics& gfx)
{
    for (auto it = boxCollidersToDraw.begin(); it != boxCollidersToDraw.end(); ++it)
    {
        SolidBox box(gfx, DirectX::XMFLOAT3(it->first->GetTransformedCenter()), DirectX::XMFLOAT3(it->first->GetTransformedSize()));
		box.SetPos(DirectX::XMFLOAT3(it->first->GetTransformedCenter().x,
			it->first->GetTransformedCenter().y,
			it->first->GetTransformedCenter().z));
        box.SetSize(DirectX::XMFLOAT3(it->first->GetTransformedSize().x,
            it->first->GetTransformedSize().y,
            it->first->GetTransformedSize().z));
        box.Draw(gfx);
        
    }

}

void App::AddCapsuleColliderToDraw(Graphics& gfx, CapsuleCollider* capsule)
{
	capsuleCollidersToDraw[capsule] = SolidSphere(gfx, 10.0f);
}

void App::DrawCapsuleColliders(Graphics& gfx)
{
    for (auto it = capsuleCollidersToDraw.begin(); it != capsuleCollidersToDraw.end(); ++it)
    {
        SolidCapsule capsule(gfx, it->first->GetTransformedBase(), it->first->GetTransformedTip(), it->first->GetRadius());
        capsule.SetBase(it->first->GetTransformedBase());
		capsule.SetTip(it->first->GetTransformedTip());
		capsule.SetRadius(it->first->GetRadius());
        capsule.Draw(gfx);
    }
}