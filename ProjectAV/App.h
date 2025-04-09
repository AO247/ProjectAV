#pragma once
#include "Window.h"
#include "Timer.h"
<<<<<<< Updated upstream
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <memory> // For unique_ptr
#include "Node.h" // Include the new Node class

// Forward declarations (if needed)

=======
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <WICTextureLoader.h>
#include <CommonStates.h>
#include <vector>
#include <memory>
#include <random>
>>>>>>> Stashed changes

class App
{
public:
    App();
<<<<<<< Updated upstream
    // master frame / message loop
    int Go();
    ~App();
private:
    void DoFrame(float dt); // Pass delta time
    void HandleInput(float dt);
    void ShowControlWindows();

private:
    // Core App Systems
    ImguiManager imgui;
    Window wnd;
    Timer timer;
    float speed_factor = 1.0f;
    Camera cam;
    PointLight light; // Keep the light as is for now
    DirectX::XMFLOAT3 nanosuitPos = { 0.0f, 0.0f, 0.0f }; // Initial position

    // --- Scene Graph ---
    std::unique_ptr<Node> pSceneRoot; // Root of the scene graph
    Node* pNanosuitNode = nullptr;    // Pointer to the specific node holding the nanosuit model (optional, for easy access)
    Node* pNanosuitNode2 = nullptr;    // Pointer to the specific node holding the nanosuit model (optional, for easy access)
    Node* pBox = nullptr;    // Pointer to the specific node holding the nanosuit model (optional, for easy access)
    Node* pEmptyNode = nullptr;
    Node* pSelectedSceneNode = nullptr; // Add this pointer

    // --- UI State ---
    bool showDemoWindow = false;
    bool cursorEnabled = false; // Track cursor state explicitly
	bool showControlWindow = true; // Show control window by default
=======
    int Go();
private:
    void DoFrame();
    void LoadAnimationFrames();
    void LoadStaticSprite();
    void LoadBackgroundLayers();
    void UpdateBackground(float deltaTime);

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
>>>>>>> Stashed changes
};