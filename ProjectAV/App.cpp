#include "App.h"
<<<<<<< Updated upstream
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
    auto pBoxOwner = std::make_unique<Node>("Box");
    pBox = pBoxOwner.get(); // Store non-owning pointer for easy access
	auto pEmptyNode = std::make_unique<Node>("EmptyNode");

    pNanosuitNode2->AddComponent(
        std::make_unique<ModelComponent>(pNanosuitNode2, wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj")
    );
    pBox->AddComponent(
        std::make_unique<ModelComponent>(pBox, wnd.Gfx(), "Models\\box.glb")
    );
	pBox->SetLocalScale(dx::XMFLOAT3(20.0f, 0.1f, 20.0f));
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
    pSceneRoot->AddChild(std::move(pBoxOwner));
	pNanosuitNode->AddChild(std::move(pNanosuitOwner2));
    pNanosuitNode2->SetLocalPosition(DirectX::XMFLOAT3(-20.0f,0.0f,0.0f));
	pSceneRoot->AddChild(std::move(pEmptyNode));
    // Initialize cursor state
    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
    cursorEnabled = false;
}

App::~App() = default; // Destructor handles unique_ptrs automatically
=======
#include <DirectXMath.h>
#include <sstream>

App::App()
    : wnd(800, 600, "ProjectAV"),
    currentNumber(1),
    gen(rd()),
    dist(0.5f, 2.0f) // For random scroll speeds
{
    auto& gfx = wnd.Gfx();

    // Initialize sprite components
    spriteBatch = std::make_unique<DirectX::SpriteBatch>(gfx.GetContext());
    spriteFont = std::make_unique<DirectX::SpriteFont>(gfx.GetDevice(), L"myfile.spritefont");
    states = std::make_unique<DirectX::CommonStates>(gfx.GetDevice());

    // Set animation position
    animPosition = DirectX::XMFLOAT2(100.f, 100.f);

    // Load assets
    LoadAnimationFrames();
    LoadStaticSprite();
    LoadBackgroundLayers();
}

void App::LoadBackgroundLayers()
{
    auto& gfx = wnd.Gfx();

    // Load 3 background layers with different scrolling speeds
    const std::vector<std::wstring> bgFiles = {
        L"bg_layer1png.png", 
        L"bg_layer2png.png",  
        L"bg_layer3png.png"   
    };

    for (size_t i = 0; i < bgFiles.size(); i++)
    {
        BackgroundLayer layer;

        // Load texture
        DirectX::CreateWICTextureFromFile(
            gfx.GetDevice(),
            gfx.GetContext(),
            bgFiles[i].c_str(),
            nullptr,
            layer.texture.ReleaseAndGetAddressOf()
        );

        // Set initial positions
        layer.position1 = DirectX::XMFLOAT2(0.f, 0.f);
        layer.position2 = DirectX::XMFLOAT2(wnd.GetWidth(), 0.f);

        // Different scroll speeds for parallax effect
        layer.scrollSpeed = 50.f + (i * 40.f); // Faster for closer layers
        layer.scale = 1.0f - (i * 0.1f);      // Slightly smaller for distant layers

        backgroundLayers.push_back(layer);
    }
}

void App::UpdateBackground(float deltaTime)
{
    for (auto& layer : backgroundLayers)
    {
        // Move both background instances
        layer.position1.x -= layer.scrollSpeed * deltaTime;
        layer.position2.x -= layer.scrollSpeed * deltaTime;

        // Reset position when fully scrolled
        if (layer.position1.x + wnd.GetWidth() < 0)
            layer.position1.x = layer.position2.x + wnd.GetWidth();

        if (layer.position2.x + wnd.GetWidth() < 0)
            layer.position2.x = layer.position1.x + wnd.GetWidth();
    }
}

// ... (Keep existing LoadAnimationFrames and LoadStaticSprite implementations)
>>>>>>> Stashed changes

int App::Go()
{
    while (true)
    {
<<<<<<< Updated upstream
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
=======
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        DoFrame();
>>>>>>> Stashed changes
    }
}

void App::HandleInput(float dt)
{
<<<<<<< Updated upstream
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
=======
    float deltaTime = timer.Mark();

    const float t = sin(timer.Peek()) / 2.0f + 0.5f;
    wnd.Gfx().ClearBuffer(t, t, 1.0f);

    // Update background
    UpdateBackground(deltaTime);

    // Update animation
    accumulator += deltaTime;
    if (accumulator >= frameTime)
    {
        accumulator = 0.0f;
        currentFrame = (currentFrame + 1) % animationFrames.size();
    }

    // Update counter
    currentNumber += 1;
    std::wstringstream ss;
    ss << currentNumber;
    std::wstring numberStr = ss.str();

    // Begin sprite batch
    spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, states->NonPremultiplied());

    // 1. Draw background layers first (farthest back)
    for (const auto& layer : backgroundLayers)
    {
        spriteBatch->Draw(
            layer.texture.Get(),
            layer.position1,
            nullptr,
            DirectX::Colors::White,
            0.f,
            DirectX::XMFLOAT2(0.f, 0.f),
            layer.scale
        );

        spriteBatch->Draw(
            layer.texture.Get(),
            layer.position2,
            nullptr,
            DirectX::Colors::White,
            0.f,
            DirectX::XMFLOAT2(0.f, 0.f),
            layer.scale
        );
    }

    // 2. Draw test triangle (if needed)
    wnd.Gfx().DrawTestTriangle();

    // 3. Draw animation (middle layer)
    if (!animationFrames.empty())
    {
        spriteBatch->Draw(
            animationFrames[currentFrame].Get(),
            animPosition,
            nullptr,
            DirectX::Colors::White,
            0.f,
            DirectX::XMFLOAT2(0.f, 0.f),
            1.0f
        );
    }

    // 4. Draw counter text (on top of background and animation)
    spriteFont->DrawString(spriteBatch.get(), numberStr.c_str(), DirectX::XMFLOAT2(10.f, 10.f));

    // 5. Draw static sprite LAST (will appear on top of everything)
    if (staticSprite)
    {
        spriteBatch->Draw(
            staticSprite.Get(),
            staticSpritePosition,
            nullptr,
            DirectX::Colors::White,
            0.f,
            DirectX::XMFLOAT2(0.f, 0.f),
            1.0f
        );
    }

    spriteBatch->End();

    wnd.Gfx().EndFrame();
}

void App::LoadAnimationFrames()  // Implementation
{
    auto& gfx = wnd.Gfx();

    // List of animation frame filenames - adjust to match your actual files
    const std::vector<std::wstring> frameFiles = {
        L"frame1.png",
        L"frame2.png",
        L"frame3.png",
        L"frame4.png",
        L"frame5.png"
    };

    // Load each frame
    for (const auto& file : frameFiles)
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> frame;
        HRESULT hr = DirectX::CreateWICTextureFromFile(
            gfx.GetDevice(),
            gfx.GetContext(),
            file.c_str(),
            nullptr,
            frame.ReleaseAndGetAddressOf()
        );

        if (SUCCEEDED(hr))
        {
            animationFrames.push_back(frame);
        }
        else
        {
            // Handle error - maybe output debug message
            OutputDebugStringW(L"Failed to load animation frame: ");
            OutputDebugStringW(file.c_str());
            OutputDebugStringW(L"\n");
        }
    }
}

void App::LoadStaticSprite()
{
    auto& gfx = wnd.Gfx();

    // Load static sprite texture
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        gfx.GetDevice(),
        gfx.GetContext(),
        L"hud_element.png",  // Make sure this file exists in your project
        nullptr,
        staticSprite.ReleaseAndGetAddressOf()
    );

    if (FAILED(hr))
    {
        // Error handling - output to debug console
        OutputDebugStringW(L"Failed to load static sprite texture\n");
        // You might want to throw an exception or handle this differently
        return;
    }

    // Get texture dimensions to calculate position
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    staticSprite->GetResource(&resource);

    D3D11_TEXTURE2D_DESC desc;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2D;
    resource.As(&texture2D);
    texture2D->GetDesc(&desc);

    // Position in bottom-right corner with padding
    staticSpritePosition = DirectX::XMFLOAT2(
        wnd.GetWidth() - desc.Width - 20.0f,  // Right side minus width and padding
        wnd.GetHeight() - desc.Height - 20.0f  // Bottom minus height and padding
    );
}
>>>>>>> Stashed changes
