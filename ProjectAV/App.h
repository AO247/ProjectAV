#pragma once
#include "Window.h"
#include "Timer.h"
#include "Button.h"
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <memory>
#include <random>
#include "Mouse.h"

class App
{
public:
    App();
    int Go();
private:
    void DoFrame();
    void LoadAnimationFrames();
    void LoadStaticSprite();
    void LoadBackgroundLayers();
    void UpdateBackground(float deltaTime);
    void ToggleBackgroundDirection();


    Window wnd;
    Timer timer;
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;
    std::unique_ptr<DirectX::CommonStates> states;

    // Animation variables
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> animationFrames;
    DirectX::XMFLOAT2 animPosition;
    float frameTime = 0.1f;
    float accumulator = 0.0f;
    size_t currentFrame = 0;

    // Static sprite variables
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> staticSprite;
    DirectX::XMFLOAT2 staticSpritePosition;

    // Background variables
    struct BackgroundLayer {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
        DirectX::XMFLOAT2 position1;
        DirectX::XMFLOAT2 position2;
        float scrollSpeed;
        float scale;
    };
    std::vector<BackgroundLayer> backgroundLayers;

    // Random number generation
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist;

    // Counter variable
    int currentNumber;

    // Button
    Mouse mouse;
    std::unique_ptr<Button> directionButton;
    bool backgroundMovingRight = false;
    void PrintMousePosition() const;

};