#include "App.h"
#include "Node.h"           
#include "ModelComponent.h" 
#include "PlayerController.h" // Include PlayerController header
#include "CMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <memory>
#include <algorithm>

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
    :
    wnd(1280, 720, "Project AV - FPS Controller"), // Pass window dimensions/title
    light(wnd.Gfx()),
    pSceneRoot(std::make_unique<Node>("Root"))
{
    // Set Projection Matrix (Far plane adjusted for larger scenes potentially)
    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 2000.0f));

    // --- Create Player Node ---
   




    auto pPlayerNodeOwner = std::make_unique<Node>("Player");
    pPlayerNode = pPlayerNodeOwner.get();
    //auto pNanosuitOwner = std::make_unique<Node>("Nanosuit");
    //pNanosuitNode = pNanosuitOwner.get();
    //auto pNanosuitOwner2 = std::make_unique<Node>("Nanosuit2");
    //pNanosuitNode2 = pNanosuitOwner2.get();
    auto pEmptyNode = std::make_unique<Node>("EmptyNode");
    /*auto pBrickOwner = std::make_unique<Node>("Brick");
    pBrick = pBrickOwner.get();
    auto pBoxOwner = std::make_unique<Node>("Box");
    pBox = pBoxOwner.get();
    auto pStoneOwner = std::make_unique<Node>("Stone");
    pStone = pStoneOwner.get();*/
    auto pColumnOwner = std::make_unique<Node>("Column");
    pColumn = pColumnOwner.get();
    //auto pIslandOwner = std::make_unique<Node>("Island");
    //pIsland = pIslandOwner.get();


    // Adding Components
    pPlayerNode->AddComponent(
		std::make_unique<PlayerController>(pPlayerNode, wnd)
    );
    /*pNanosuitNode->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );
    pNanosuitNode2->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode2, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );*/
    /*pBrick->AddComponent(
        std::make_unique<ModelComponent>(pBrick, wnd.Gfx(), "Models\\brick_wall\\brick_wall.obj")
    );
    pBox->AddComponent(
        std::make_unique<ModelComponent>(pBox, wnd.Gfx(), "Models\\box.glb")
    );*/
    /*pStone->AddComponent(
		std::make_unique<ModelComponent>(pStone, wnd.Gfx(), "Models\\kamien\\kamien2.glb")
    );*/
    pColumn->AddComponent(
        std::make_unique<ModelComponent>(pColumn, wnd.Gfx(), "Models\\kolumna\\kolumna.obj")
    );
    //pIsland->AddComponent(
    //    std::make_unique<ModelComponent>(pIsland, wnd.Gfx(), "Models\\wyspa\\wyspa.obj")
    //);



    // Adding to Scene Graph
    pSceneRoot->AddChild(std::move(pPlayerNodeOwner));
    //pSceneRoot->AddChild(std::move(pNanosuitOwner));
    //pSceneRoot->AddChild(std::move(pNanosuitOwner2));
    pSceneRoot->AddChild(std::move(pEmptyNode));
    /*pSceneRoot->AddChild(std::move(pBrickOwner));
    pSceneRoot->AddChild(std::move(pBoxOwner));
    pSceneRoot->AddChild(std::move(pStoneOwner));*/
    pSceneRoot->AddChild(std::move(pColumnOwner));
    //pSceneRoot->AddChild(std::move(pIslandOwner));

    // Changing position scale etc.
    pPlayerNode->SetLocalPosition({ 0.0f, 5.0f, -10.0f });
    //pNanosuitNode2->SetLocalPosition(DirectX::XMFLOAT3(-20.0f, 0.0f, 0.0f));
    //pBrick->SetLocalScale(dx::XMFLOAT3(20.0f, 20.0f, 1.0f));
    //pBrick->SetLocalRotation(dx::XMFLOAT3(DirectX::XMConvertToRadians(90), 0.0f, 0.0f));
    //pIsland->SetLocalPosition(DirectX::XMFLOAT3(0.0f, -20.0f, 0.0f));



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

    if (pPlayerNode->GetComponent<PlayerController>())
    {
        dx::XMMATRIX playerWorldTransform = pPlayerNode->GetWorldTransform();

        dx::XMFLOAT2 playerRotation = pPlayerNode->GetComponent<PlayerController>()->GetRotation(); // {pitch, yaw}


        dx::XMVECTOR camPosition = playerWorldTransform.r[3];

        const dx::XMVECTOR forwardBaseVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        const auto lookVector = dx::XMVector3Transform(forwardBaseVector,
            dx::XMMatrixRotationRollPitchYaw(playerRotation.x, playerRotation.y, 0.0f) // Use controller's Pitch & Yaw
        );

        const auto camTarget = dx::XMVectorAdd(camPosition, lookVector);

        const dx::XMMATRIX viewMatrix = dx::XMMatrixLookAtLH(
            camPosition,
            camTarget,
            dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) // World Up vector
        );

        wnd.Gfx().SetCamera(viewMatrix);
    }
    else {
        wnd.Gfx().SetCamera(dx::XMMatrixIdentity());
    }


    light.Bind(wnd.Gfx(), wnd.Gfx().GetCamera()); // Pass the view matrix for light calculations

    pSceneRoot->Draw(wnd.Gfx());

    light.Draw(wnd.Gfx());


    if (showControlWindow) {
        ShowControlWindows();
    }

    wnd.Gfx().EndFrame();
}


void App::ShowControlWindows()
{
    // --- Existing Windows ---
    light.SpawnControlWindow();
    if (showDemoWindow)
    {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    // --- Show Model Component Windows ---
    /*if (pNanosuitNode)
    {
        if (auto* modelComp = pNanosuitNode->GetComponent<ModelComponent>())
        {
            modelComp->ShowWindow("Nanosuit Controls");
        }
    }
    if (pNanosuitNode2)
    {
        if (auto* modelComp = pNanosuitNode2->GetComponent<ModelComponent>())
        {
            modelComp->ShowWindow("Nanosuit2 Controls");
        }
    }*/

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
                if (components.empty())
                {
                    ImGui::TextDisabled("No components attached.");
                }
                else
                {
                    for (const auto& comp : components)
                    {
                        if (comp) // Check if component pointer is valid
                        {
                            // Use typeid to get a (potentially mangled) name
                            // For cleaner names, add a virtual GetTypeName() to Component base class
                            ImGui::BulletText(typeid(*comp).name());
                            // You could add specific UI for known component types here later:
                            // if (auto* modelComp = dynamic_cast<ModelComponent*>(comp.get())) { ... }
                            // if (auto* lightComp = dynamic_cast<LightComponent*>(comp.get())) { ... }
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