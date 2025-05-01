#include "App.h"
#include "Node.h"           
#include "State.h"
#include "CMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <memory>
#include "NormalMapTwerker.h"
#include <shellapi.h>
#include <algorithm>
#include "ColliderSphere.h"

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
        if (nArgs >= 4 && std::wstring(pArgs[1]) == L"--ntwerk-rotx180")
        {
            const std::wstring pathInWide = pArgs[2];
            const std::wstring pathOutWide = pArgs[3];
            NormalMapTwerker::RotateXAxis180(
                std::string(pathInWide.begin(), pathInWide.end()),
                std::string(pathOutWide.begin(), pathOutWide.end())
            );
            throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
        }
    }
	// Initialize Physics Engine
	physicsEngine = PhysicsEngine();
    
    // --- Create Nodes ---
   

    auto pPlayerNodeOwner = std::make_unique<Node>("Player");
    pPlayerNode = pPlayerNodeOwner.get();
	pPlayerNode->tag = "Player";
    auto pNanosuitOwner = std::make_unique<Node>("Nanosuit");
    pNanosuitNode = pNanosuitOwner.get();
    auto pNanosuitOwner2 = std::make_unique<Node>("Nanosuit2");
    pNanosuitNode2 = pNanosuitOwner2.get();
    auto pEmptyNode = std::make_unique<Node>("EmptyNode");
    auto pBrickOwner = std::make_unique<Node>("Brick");
    pBrick = pBrickOwner.get();
    auto pBoxOwner = std::make_unique<Node>("Box");
    pBox = pBoxOwner.get();
    auto pStoneOwner = std::make_unique<Node>("Stone");
    pStone = pStoneOwner.get();
    auto pColumnOwner = std::make_unique<Node>("Column");
    pColumn = pColumnOwner.get();
    auto pIslandOwner = std::make_unique<Node>("Island");
    pIsland = pIslandOwner.get();
	auto pNoxTurnOwner = std::make_unique<Node>("NoxTurn");
	pNoxTurn = pNoxTurnOwner.get();
	auto pNoxTurnHairOwner = std::make_unique<Node>("NoxTurnHair");
	pNoxTurnHair = pNoxTurnHairOwner.get();
	auto pTestModelOwner = std::make_unique<Node>("TestModel");
	pTestModel = pTestModelOwner.get();
	auto pEnemyOwner = std::make_unique<Node>("Enemy");
	pEnemy = pEnemyOwner.get();
	pEnemy->tag = "Enemy";

    // Adding to Scene Graph
    pSceneRoot->AddChild(std::move(pPlayerNodeOwner));
    //pSceneRoot->AddChild(std::move(pNanosuitOwner));
    //pSceneRoot->AddChild(std::move(pNanosuitOwner2));
    //pSceneRoot->AddChild(std::move(pEmptyNode));
    //pSceneRoot->AddChild(std::move(pBrickOwner));
    pSceneRoot->AddChild(std::move(pBoxOwner));
    pSceneRoot->AddChild(std::move(pStoneOwner));
    pSceneRoot->AddChild(std::move(pColumnOwner));
    pSceneRoot->AddChild(std::move(pIslandOwner));
    //pSceneRoot->AddChild(std::move(pNoxTurnOwner));
    //pNoxTurn->AddChild(std::move(pNoxTurnHairOwner));
    //pSceneRoot->AddChild(std::move(pTestModelOwner));
    pSceneRoot->AddChild(std::move(pEnemyOwner));


    // Adding Models
    pNanosuitNode->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );
    pNanosuitNode2->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode2, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
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

    pIsland->AddComponent(
        std::make_unique<ModelComponent>(pIsland, wnd.Gfx(), "Models\\wyspa\\wyspa_test.fbx")
    );


    pNoxTurn->AddComponent(
        std::make_unique<ModelComponent>(pNoxTurn, wnd.Gfx(), "Models\\stone\\char.fbx")
    );
    pNoxTurnHair->AddComponent(
        std::make_unique<ModelComponent>(pNoxTurnHair, wnd.Gfx(), "Models\\stone\\hair.fbx")
    );
	pTestModel->AddComponent(
		std::make_unique<ModelComponent>(pTestModel, wnd.Gfx(), "Models\\stone\\grave5.fbx")
	);
    pEnemy->AddComponent(
        std::make_unique<ModelComponent>(pEnemy, wnd.Gfx(), "Models\\enemy\\basic.obj")
    );



	//Adding Rigidbody and Collider
    pPlayerNode->AddComponent(
        std::make_unique<Rigidbody>(pPlayerNode, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* pRigidbody = pPlayerNode->GetComponent<Rigidbody>();
    pPlayerNode->AddComponent(
        std::make_unique<OBB>(pPlayerNode, pRigidbody, Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 3.0f, 1.0f))
    );
    OBB* pOBB = pPlayerNode->GetComponent<OBB>();
    pRigidbody->SetCollider(pOBB);
	pRigidbody->SetMass(10.0f);
    physicsEngine.AddRigidbody(pRigidbody);
    

    pEnemy->AddComponent(
        std::make_unique<Rigidbody>(pEnemy, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* eRigidbody = pEnemy->GetComponent<Rigidbody>();
    pEnemy->AddComponent(
        std::make_unique<OBB>(pEnemy, eRigidbody, Vector3(-0.7f, 2.0f, -0.5f), Vector3(1.0f, 4.0f, 1.0f))
    );
    OBB* eOBB = pEnemy->GetComponent<OBB>();
    eRigidbody->SetCollider(eOBB);
    physicsEngine.AddRigidbody(eRigidbody);


    pIsland->AddComponent(
        std::make_unique<Rigidbody>(pIsland, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
	Rigidbody* iRigidbody = pIsland->GetComponent<Rigidbody>();
	iRigidbody->SetStatic(true);
    pIsland->AddComponent(
        std::make_unique<OBB>(pIsland, iRigidbody, Vector3(0.0f, 0.0f, 0.0f), Vector3(50.0f, 1.0f, 50.0f))
    );
	OBB* iOBB = pIsland->GetComponent<OBB>();
	iRigidbody->SetCollider(iOBB);
	physicsEngine.AddRigidbody(iRigidbody);



	pBox->AddComponent(
		std::make_unique<Rigidbody>(pBox, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
	);
	Rigidbody* bRigidbody = pBox->GetComponent<Rigidbody>();
	bRigidbody->SetStatic(true);
	pBox->AddComponent(
		std::make_unique<BoundingSphere>(pBox, Vector3(0.0f, 0.0f, 0.0f), 2.0f, bRigidbody)
	);
	BoundingSphere* bBoundingSphere = pBox->GetComponent<BoundingSphere>();
	bRigidbody->SetCollider(bBoundingSphere);
	physicsEngine.AddRigidbody(bRigidbody);


	pColumn->AddComponent(
		std::make_unique<Rigidbody>(pColumn, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
	);
	Rigidbody* cRigidbody = pColumn->GetComponent<Rigidbody>();
	cRigidbody->SetStatic(true);
	pColumn->AddComponent(
		std::make_unique<OBB>(pColumn, cRigidbody, Vector3(-0.6f, 5.0f, 0.0f), Vector3(2.9f, 10.0f, 2.9f))
	);
	OBB* cOBB = pColumn->GetComponent<OBB>();
	cRigidbody->SetCollider(cOBB);
	physicsEngine.AddRigidbody(cRigidbody);

    //Adding Other Components

    pPlayerNode->AddComponent(
        std::make_unique<PlayerController>(pPlayerNode, wnd) // Add controller first
    );
	pEnemy->AddComponent(
		std::make_unique<StateMachine>(pEnemy, StateType::IDLE)
	);
    pEnemy->GetComponent<StateMachine>()->AddComponent(
        std::make_unique<Walking>(pEnemy)
    );
    pEnemy->GetComponent<StateMachine>()->UpdateComponents();





    // Changing position scale etc.
    pPlayerNode->SetLocalPosition({ 0.0f, 35.0f, 0.0f });
    pNanosuitNode2->SetLocalPosition(DirectX::XMFLOAT3(-20.0f, 0.0f, 0.0f));
	pBox->SetLocalPosition(DirectX::XMFLOAT3(10.0f, 3.0f, 0.0f));
    pBrick->SetLocalScale(dx::XMFLOAT3(20.0f, 20.0f, 1.0f));
    pBrick->SetLocalRotation(dx::XMFLOAT3(DirectX::XMConvertToRadians(90), 0.0f, 0.0f));
	pStone->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	pStone->SetLocalScale(dx::XMFLOAT3(1.5f, 1.5f, 1.5f));
    pIsland->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	pIsland->SetLocalScale(dx::XMFLOAT3(1.3f, 1.3f, 1.3f));
	pNoxTurn->SetLocalPosition(DirectX::XMFLOAT3(5.0f, 0.0f, 5.0f));
	pNoxTurn->SetLocalScale(dx::XMFLOAT3(0.01f, 0.01f, 0.01f));
	pTestModel->SetLocalPosition({ -5.0f, 0.0f, -5.0f });
	pTestModel->SetLocalScale(dx::XMFLOAT3(0.01f, 0.01f, 0.01f));
	pEnemy->SetLocalPosition(DirectX::XMFLOAT3(15.0f, 10.0f, 0.0f));
	pColumn->SetLocalPosition(DirectX::XMFLOAT3(-7.0f, 0.0f, -5.0f));
    //Adding colliders to draw
    AddBoxColliderToDraw(wnd.Gfx(), pOBB);
    AddBoxColliderToDraw(wnd.Gfx(), iOBB);
    AddBoxColliderToDraw(wnd.Gfx(), eOBB);
	AddBoxColliderToDraw(wnd.Gfx(), cOBB);


	AddSphereColliderToDraw(wnd.Gfx(), bBoundingSphere);


    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
    cursorEnabled = false;
}

App::~App() = default;

int App::Go()
{
    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        const auto dt = timer.Mark() * speed_factor;

        physicsEngine.Simulate(dt);
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
        }
    }

}

void App::DoFrame(float dt)
{
    pSceneRoot->Update(dt);

    wnd.Gfx().BeginFrame(0.5f, 0.5f, 1.0f);
    if (pPlayerNode->GetLocalPosition().y < -10.0f) {
		pPlayerNode->SetLocalPosition({ 0.0f, 15.0f, 0.0f });
        pEnemy->SetLocalPosition({ 15.0f, 30.0f, 0.0f });
    }
    dx::XMMATRIX viewMatrix = dx::XMMatrixIdentity(); // Default
    if (pPlayerNode && pPlayerNode->GetComponent<PlayerController>())
    {
        dx::XMMATRIX playerWorldTransform = pPlayerNode->GetWorldTransform();
        dx::XMFLOAT2 playerRotation = pPlayerNode->GetComponent<PlayerController>()->GetRotation();
        dx::XMVECTOR camPosition = playerWorldTransform.r[3];
        const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const auto lookVector = dx::XMVector3Transform(forwardBaseVector, dx::XMMatrixRotationRollPitchYaw(playerRotation.x, playerRotation.y, 0.0f));
        const auto camTarget = dx::XMVectorAdd(camPosition, lookVector);
        viewMatrix = dx::XMMatrixLookAtLH(camPosition, camTarget, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }
    wnd.Gfx().SetCamera(viewMatrix);

    // --- Bind Lights ---
    pointLight.Bind(wnd.Gfx(), viewMatrix); // Bind point light (to slot 0)


    pSceneRoot->Draw(wnd.Gfx());





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
    pointLight.Draw(wnd.Gfx());

    pointLight.SpawnControlWindow(); // Control for Point Light
    if (showDemoWindow)
    {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    // --- Show Model Component Windows ---
    if (pNanosuitNode)
    {
        if (auto* modelComp = pNanosuitNode->GetComponent<ModelComponent>())
        {
            modelComp->ShowWindow("Nanosuit Controls");
        }
    }

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
        /*it->second.SetPos(DirectX::XMFLOAT3(it->first->GetCenter().x,
                                            it->first->GetCenter().y,
                                            it->first->GetCenter().z));
        it->second.Draw(gfx);*/
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
      /*  box.SetSize(DirectX::XMFLOAT3(it->first->GetTransformedSize().x,
            it->first->GetTransformedSize().y,
            it->first->GetTransformedSize().z));*/
        box.Draw(gfx);
        
    }

}