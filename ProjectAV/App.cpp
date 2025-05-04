#include "App.h"
#include "Node.h"           // Include Node
#include "ModelComponent.h" // Include ModelComponent
#include "CMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"
#include <memory>
#include <algorithm>
#include "BoundingSphere.h"
#include "ColliderSphere.h"
#include "AABB.h"
#include "Raycast.h"
//#include "Rigidbody.h"

namespace dx = DirectX;

GDIPlusManager gdipm; // Keep GDI+ manager

App::App()
    :
    wnd(1280, 720, "Project AV"),
    light(wnd.Gfx()), // Initialize light
    pSceneRoot(std::make_unique<Node>("Root")) // Create scene root node
{
    // Set Projection Matrix
    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 200.0f));

    // --- Create Scene Objects ---

    // 1. Create a Node for the Nanosuit model
    auto pNanosuitOwner = std::make_unique<Node>("Nanosuit");
    pNanosuitNode = pNanosuitOwner.get(); // Store non-owning pointer for easy access
    auto pNanosuitOwner2 = std::make_unique<Node>("Nanosuit2");
    pNanosuitNode2 = pNanosuitOwner2.get(); // Store non-owning pointer for easy access
    auto pBoxOwner = std::make_unique<Node>("Box");
    pBox = pBoxOwner.get(); // Store non-owning pointer for easy access
	auto pEmptyNode = std::make_unique<Node>("EmptyNode");

    Raycast::physicsEngine = &physicsEngine;

    pNanosuitNode2->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode2, wnd.Gfx(), "Models\\Colliders\\Sphere.obj")
    );
    pNanosuitNode2->AddComponent(
        std::make_unique<Rigidbody>(pNanosuitNode2, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* rb1 = pNanosuitNode2->GetComponent<Rigidbody>();
    pNanosuitNode2->AddComponent(
        std::make_unique<BoundingSphere>(pNanosuitNode2, Vector3(0,0,0), 1.0f, rb1)
    );
    BoundingSphere* bs1 = pNanosuitNode2->GetComponent<BoundingSphere>();
    bs1->SetIsTrigger(true);
    rb1->SetMass(10);
    AddSphereColliderToDraw(wnd.Gfx(), bs1);
    rb1->SetCollider(bs1);
    physicsEngine.AddRigidbody(rb1);

    pBox->AddComponent(
        std::make_unique<ModelComponent>(pBox, wnd.Gfx(), "Models\\box.glb")
    );
    pBox->AddComponent(
        std::make_unique<Rigidbody>(pBox, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* brb = pBox->GetComponent<Rigidbody>();
    brb->SetStatic(true);
    pBox->AddComponent(
        std::make_unique<OBB>(pBox, brb, Vector3(0, 0, 0), Vector3(2, 2, 2))
    );
    OBB* baabb = pBox->GetComponent<OBB>();
    //AddBoxColliderToDraw(wnd.Gfx(), baabb);
    brb->SetCollider(baabb);
    physicsEngine.AddRigidbody(brb);
	pBox->SetLocalScale(dx::XMFLOAT3(20.0f, 1.0f, 20.0f));
    // 2. Add the ModelComponent to the Nanosuit Node
    try {
        // The ModelComponent constructor now takes the Node* owner
        pNanosuitNode->AddComponent(
            std::make_unique<ModelComponent>(pNanosuitNode, wnd.Gfx(), "Models\\Colliders\\Box.obj")
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

    pNanosuitNode->AddComponent(
        std::make_unique<Rigidbody>(pNanosuitNode, Vector3(-10.0f, 30.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f))
    );
    Rigidbody* rb2 = pNanosuitNode->GetComponent<Rigidbody>();
    //pNanosuitNode->AddComponent(
    //    std::make_unique<OBB>(pNanosuitNode, rb2, Vector3(0, 0, 0), Vector3(2, 2, 2))
    //);
    //OBB* bs2 = pNanosuitNode->GetComponent<OBB>();
    //rb2->SetCollider(bs2);
    pNanosuitNode->AddComponent(
        std::make_unique<CapsuleCollider>(pNanosuitNode, rb2, 2, Vector3(0, -2, 0), Vector3(0, 6, 0))
    );
    CapsuleCollider* cc = pNanosuitNode->GetComponent<CapsuleCollider>();
    rb2->SetCollider(cc);
    physicsEngine.AddRigidbody(rb2);
    AddCapsuleColliderToDraw(wnd.Gfx(), cc);

    // 3. Add the Nanosuit Node to the Scene Root
    pEmptyNode->AddChild(std::move(pNanosuitOwner));
    pSceneRoot->AddChild(std::move(pBoxOwner));
    pSceneRoot->AddChild(std::move(pNanosuitOwner2));
    pNanosuitNode2->SetLocalPosition(DirectX::XMFLOAT3(15.0f, 3.0f, 0.0f));
    pNanosuitNode->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f));
	pSceneRoot->AddChild(std::move(pEmptyNode));
    // Initialize cursor state
    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
    cursorEnabled = false;
}

App::~App() = default; // Destructor handles unique_ptrs automatically

int App::Go()
{
    float lag = 0.0f;
    float FIXED_TIME_STEP = 0.017f;
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
        lag += dt;
        
        while (lag >= FIXED_TIME_STEP)
        {
            physicsEngine.Simulate(FIXED_TIME_STEP);
            lag -= FIXED_TIME_STEP;
        }

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
    //pNanosuitNode->SetLocalPosition(DirectX::XMFLOAT3(pNanosuitNode->GetLocalPosition().x + dt, 0.0f, 0.0f));
    pSceneRoot->Update(dt);
    if (wnd.kbd.KeyIsPressed('X'))
    {
        pNanosuitNode->GetComponent<Rigidbody>()->AddForce(Vector3(-200.0f, 0.0f, 0.0f));
    }
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

    DrawSphereColliders(wnd.Gfx());
    DrawBoxColliders(wnd.Gfx());

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

void App::AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere)
{
    sphereCollidersToDraw[boundingSphere] = SolidSphere(gfx, boundingSphere->GetRadius());
}

void App::AddCapsuleColliderToDraw(Graphics& gfx, CapsuleCollider* capsuleCollider)
{
    capsuleCollidersToDraw[capsuleCollider] = SolidSphere(gfx, capsuleCollider->GetRadius());
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
    for (auto it = capsuleCollidersToDraw.begin(); it != capsuleCollidersToDraw.end(); ++it)
    {
        ColliderSphere tsphere(gfx, it->first->GetRadius());
        tsphere.SetPos(DirectX::XMFLOAT3(it->first->GetTransformedTip().x,
            it->first->GetTransformedTip().y,
            it->first->GetTransformedTip().z));
        tsphere.Draw(gfx);

        ColliderSphere bsphere(gfx, it->first->GetRadius());
        bsphere.SetPos(DirectX::XMFLOAT3(it->first->GetTransformedBase().x,
            it->first->GetTransformedBase().y,
            it->first->GetTransformedBase().z));
        bsphere.Draw(gfx);
    }
}

void App::AddBoxColliderToDraw(Graphics& gfx, AxisAligned::AABB* aabb)
{
    boxCollidersToDraw[aabb] = SolidBox(gfx, DirectX::XMFLOAT3(aabb->GetTransformedExtents(aabb->GetMinExtents()).x,
                                                               aabb->GetTransformedExtents(aabb->GetMinExtents()).y,
                                                               aabb->GetTransformedExtents(aabb->GetMinExtents()).z),
                                             DirectX::XMFLOAT3(aabb->GetTransformedExtents(aabb->GetMaxExtents()).x,
                                                               aabb->GetTransformedExtents(aabb->GetMaxExtents()).y,
                                                               aabb->GetTransformedExtents(aabb->GetMaxExtents()).z));
}

void App::DrawBoxColliders(Graphics& gfx)
{
    for (auto it = boxCollidersToDraw.begin(); it != boxCollidersToDraw.end(); ++it)
    {
        //SolidBox box(gfx, DirectX::XMFLOAT3(it->first->GetTransformedExtents(it->first->GetMinExtents()).x,
        //                                    it->first->GetTransformedExtents(it->first->GetMinExtents()).y,
        //                                    it->first->GetTransformedExtents(it->first->GetMinExtents()).z),
        //                                    DirectX::XMFLOAT3(it->first->GetTransformedExtents(it->first->GetMaxExtents()).x,
        //                                                      it->first->GetTransformedExtents(it->first->GetMaxExtents()).y,
        //                                                      it->first->GetTransformedExtents(it->first->GetMaxExtents()).z));
        SolidBox box(gfx, DirectX::XMFLOAT3(it->first->GetMinExtents().x,
                                            it->first->GetMinExtents().y,
                                            it->first->GetMinExtents().z),
                                            DirectX::XMFLOAT3(it->first->GetMaxExtents().x,
                                                              it->first->GetMaxExtents().y,
                                                              it->first->GetMaxExtents().z));
        box.SetTransformXM(it->first->GetRigidbody()->GetTransformationMatrixFromNode());
        box.Draw(gfx);
    }
}