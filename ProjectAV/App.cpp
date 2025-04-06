#include "App.h"
#include "Node.h"           // Include Node
#include "ModelComponent.h" // Include ModelComponent
#include "CMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <memory>
#include <algorithm>

namespace dx = DirectX;

GDIPlusManager gdipm; // Keep GDI+ manager

App::App()
    :
    wnd(1280, 720, "Project AV"),
    light(wnd.Gfx()), // Initialize light
    pSceneRoot(std::make_unique<Node>("Root")) // Create scene root node
{
    // Set Projection Matrix
    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 40.0f));

    // --- Create Scene Objects ---

    // 1. Create a Node for the Nanosuit model
    auto pNanosuitOwner = std::make_unique<Node>("Nanosuit");
    pNanosuitNode = pNanosuitOwner.get(); // Store non-owning pointer for easy access
    auto pNanosuitOwner2 = std::make_unique<Node>("Nanosuit2");
    pNanosuitNode2 = pNanosuitOwner2.get(); // Store non-owning pointer for easy access
    auto pNanosuitOwner3 = std::make_unique<Node>("Nanosuit3");
    pNanosuitNode3 = pNanosuitOwner3.get(); // Store non-owning pointer for easy access
	auto pEmptyNode = std::make_unique<Node>("EmptyNode");

    pNanosuitNode2->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode2, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );
    pNanosuitNode3->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode3, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );
    // 2. Add the ModelComponent to the Nanosuit Node
    try {
        // The ModelComponent constructor now takes the Node* owner
        pNanosuitNode->AddComponent(
            std::make_unique<ModelComponent>(pNanosuitNode, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
        );

    }
    catch (const CException& e)
    {
        MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
        // Handle model loading failure gracefully (e.g., exit or continue without model)
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBoxA(nullptr, "Unknown exception during model loading.", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }


    // 3. Add the Nanosuit Node to the Scene Root
    pSceneRoot->AddChild(std::move(pNanosuitOwner));
    pSceneRoot->AddChild(std::move(pNanosuitOwner3));
	pNanosuitNode->AddChild(std::move(pNanosuitOwner2));
    pNanosuitNode2->SetLocalPosition(DirectX::XMFLOAT3(-20.0f,0.0f,0.0f));
	pSceneRoot->AddChild(std::move(pEmptyNode));
    // Initialize cursor state
    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
    cursorEnabled = false;
}

App::~App() = default; // Destructor handles unique_ptrs automatically

int App::Go()
{
    while (true)
    {
        // process all messages pending, but do not block for new messages
        if (const auto ecode = Window::ProcessMessages())
        {
            // if return optional has value, means we're quitting so return exit code
            return *ecode;
        }

        // Calculate delta time
        const auto dt = timer.Mark() * speed_factor;

        // Main frame logic
        HandleInput(dt);
        DoFrame(dt);
    }
}

void App::HandleInput(float dt)
{
    // Keyboard input
    while (const auto e = wnd.kbd.ReadKey())
    {
        if (!e->IsPress()) continue;

        switch (e->GetCode())
        {
        case 'C': // Toggle cursor
            if (cursorEnabled)
            {
                wnd.DisableCursor();
                wnd.mouse.EnableRaw();
            }
            else
            {
                wnd.EnableCursor();
                wnd.mouse.DisableRaw();
            }
            cursorEnabled = !cursorEnabled; // Toggle state
            break;
        case 'H':
			showControlWindow = !showControlWindow; // Toggle control window
			break;
        case VK_ESCAPE: // Exit application
            PostQuitMessage(0);
            return;
        case VK_F1:
            showDemoWindow = !showDemoWindow; // Toggle demo window
            break;
        }
    }

    // Camera movement only when cursor is disabled
    if (!cursorEnabled)
    {
        if (wnd.kbd.KeyIsPressed('W')) cam.Translate({ 0.0f, 0.0f, dt });
        if (wnd.kbd.KeyIsPressed('S')) cam.Translate({ 0.0f, 0.0f, -dt });
        if (wnd.kbd.KeyIsPressed('A')) cam.Translate({ -dt, 0.0f, 0.0f });
        if (wnd.kbd.KeyIsPressed('D')) cam.Translate({ dt, 0.0f, 0.0f });
        if (wnd.kbd.KeyIsPressed('R')) cam.Translate({ 0.0f, dt, 0.0f }); // Up
        if (wnd.kbd.KeyIsPressed('F')) cam.Translate({ 0.0f, -dt, 0.0f }); // Down
    }

    // Mouse input for camera rotation only when cursor is disabled
    while (const auto delta = wnd.mouse.ReadRawDelta())
    {
        if (!cursorEnabled)
        {
            cam.Rotate((float)delta->x, (float)delta->y);
        }
    }
}


void App::DoFrame(float dt)
{
    // Update scene graph (updates transforms, components)
    // Keep Z the same, or modify as needed
    // Set the node's local position to the updated stored position
    pNanosuitNode->SetLocalPosition(DirectX::XMFLOAT3(pNanosuitNode->GetLocalPosition().x + dt, 0.0f, 0.0f));
    pSceneRoot->Update(dt);
    // Begin Frame
    wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f); // Clear color

    // Set Camera
    wnd.Gfx().SetCamera(cam.GetMatrix());

    // Bind Lights (if applicable)
    light.Bind(wnd.Gfx(), cam.GetMatrix());

    // --- Draw the Scene Graph ---
    // The root node's Draw call will recursively draw all children and their components
    pSceneRoot->Draw(wnd.Gfx());

    // Draw helpers (like the light representation)
    light.Draw(wnd.Gfx());

    // --- ImGui ---
    if (showControlWindow) {
        ShowControlWindows();
    }

    // Present Frame
    wnd.Gfx().EndFrame();
}

void App::ShowControlWindows()
{
    // --- Existing Windows ---
    cam.SpawnControlWindow();
    light.SpawnControlWindow();
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
    if (pNanosuitNode2)
    {
        if (auto* modelComp = pNanosuitNode2->GetComponent<ModelComponent>())
        {
            modelComp->ShowWindow("Nanosuit2 Controls");
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