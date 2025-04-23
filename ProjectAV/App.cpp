#include "App.h"
#include <DirectXMath.h>
#include <sstream>
#include <iostream>

App::App()
    : wnd(800, 600, "ProjectAV"),
    currentNumber(1),
    gen(rd()),
    dist(0.5f, 2.0f), // For random scroll speeds
    backgroundMovingRight(false)

{
    auto& gfx = wnd.Gfx();

    // Initialize sprite components
    spriteBatch = std::make_unique<DirectX::SpriteBatch>(gfx.GetContext());
    spriteFont = std::make_unique<DirectX::SpriteFont>(gfx.GetDevice(), L"myfile.spritefont");
    states = std::make_unique<DirectX::CommonStates>(gfx.GetDevice());

    // Set animation position
    animPosition = DirectX::XMFLOAT2(100.f, 100.f);

    directionButton = std::make_unique<Button>(
        L"Toggle",
        DirectX::XMFLOAT2(0.f, -10.f),
        DirectX::XMFLOAT2(200.f, 50.f),
        [this]() { ToggleBackgroundDirection(); },
        gfx.GetDevice()
    );


    // Load assets
    LoadAnimationFrames();
    LoadStaticSprite();
    LoadBackgroundLayers();
}



void App::DoFrame()
{
    float deltaTime = timer.Mark();
    //PrintMousePosition();

    const float t = sin(timer.Peek()) / 2.0f + 0.5f;
    wnd.Gfx().ClearBuffer(t, t, 1.0f);

    // Update button
    auto [mouseX, mouseY] = wnd.GetMouse().GetPos();
	//bool czuKlik = wnd.GetMouse().IsLeftPressed();
    directionButton->Update(mouseX, mouseY, wnd.GetMouse().LeftIsPressed());


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
            DirectX::XMFLOAT2(0.f, -50.f),
            layer.scale
        );

        spriteBatch->Draw(
            layer.texture.Get(),
            layer.position2,
            nullptr,
            DirectX::Colors::White,
            0.f,
            DirectX::XMFLOAT2(0.f, -50.f),
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
            DirectX::XMFLOAT2(0.f, -250.f),
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

    directionButton->Draw(spriteBatch.get());


    spriteBatch->End();

    wnd.Gfx().EndFrame();
}

void App::ToggleBackgroundDirection()
{
    backgroundMovingRight = !backgroundMovingRight;

	OutputDebugString("Background direction toggled\n");
    // Reverse all scroll speeds
    for (auto& layer : backgroundLayers)
    {
        layer.scrollSpeed = -layer.scrollSpeed;
    }
}

// Update the UpdateBackground method:
void App::UpdateBackground(float deltaTime)
{
    for (auto& layer : backgroundLayers)
    {
        // Move both background instances
        layer.position1.x -= layer.scrollSpeed * deltaTime;
        layer.position2.x -= layer.scrollSpeed * deltaTime;

        // Handle wrap-around based on direction
        if (backgroundMovingRight)
        {
            if (layer.position1.x > wnd.GetWidth())
                layer.position1.x = layer.position2.x - wnd.GetWidth();

            if (layer.position2.x > wnd.GetWidth())
                layer.position2.x = layer.position1.x - wnd.GetWidth();
        }
        else
        {
            if (layer.position1.x + wnd.GetWidth() < 0)
                layer.position1.x = layer.position2.x + wnd.GetWidth();

            if (layer.position2.x + wnd.GetWidth() < 0)
                layer.position2.x = layer.position1.x + wnd.GetWidth();
        }
    }
}


void App::LoadAnimationFrames()
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
        layer.scale = 1.0f;

        backgroundLayers.push_back(layer);
    }
}


void App::PrintMousePosition() const
{
    
    auto [x, y] = wnd.GetMouse().GetPos();;
    std::wstringstream ss;
    ss << L"Mouse position: (" << x << L", " << y << L")";
    OutputDebugStringW(ss.str().c_str());
    OutputDebugStringW(L"\n");

#ifdef _DEBUG
    std::wcout << L"Mouse position: (" << x << L", " << y << L")\n";

#endif
}

int App::Go()
{
    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }
        DoFrame();
    }
}