#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include <memory> 
#include "Node.h" 
#include <map>
#include "Components.h"
#include "MusicBuffer.h"
#include "SoundDevice.h"
#include "ColliderSphere.h"
#include "SolidBox.h"
#include "SolidCapsule.h"
#include "DebugLine.h"
#include "PhysicsCommon.h"
#include "TestCube.h"
#include "Global.h"
#include "MainRenderGraph.h"
#include "Sprite.h"
#include "Button.h"
#include "DirectionalLight.h"
#include <fstream> 

class PlayerController;

class App
{
public:
    App(const std::string& commandLine = "");
    int Go();
    ~App();

    enum class GameState {
        MainMenu,
        Gameplay,
        Paused
    };
    GameState gameState = GameState::MainMenu;


private:
    void DoFrame(float dt);
    void HandleInput(float dt);
    void ShowControlWindows();
    void ForEnemyWalking();
    void SaveNodeTransformsRecursive(Node& node, std::ofstream& file);
    void StartGame();

private:
    std::string commandLine;
    ImguiManager imgui;
    Window wnd;
    Rgph::MainRenderGraph rg{ wnd.Gfx() };
    Timer timer;
    float speed_factor = 1.0f;
    PointLight pointLight;
    DirectionalLight dirLight;

    PhysicsSystem* physicsSystem;
    TempAllocatorImpl* temp_allocator;
    JobSystemThreadPool* job_system;
    MyContactListener* contactListener;


    SoundDevice* soundDevice; 
    std::unique_ptr<MusicBuffer> myMusic;
    DirectX::BoundingFrustum cameraFrustum; 

    std::unique_ptr<Node> pSceneRoot;
    Node* pCamera = nullptr;
	Node* pFreeViewCamera = nullptr; 
    Node* pPlayer = nullptr; 
	Node* pSelectedSceneNode = nullptr; 
	Node* pSoundEffectsPlayer = nullptr;
	Node* pLeftHand = nullptr;
	Node* pRightHand = nullptr;
	Node* tutorialNode = nullptr;
    Node* temporary = nullptr; 
	Node* pAbility3 = nullptr;
    UpgradeHandler* pUpgradeHandler = nullptr;

    TestCube cube{ wnd.Gfx(),4.0f };
    void FrustumCalculating();
    void DrawNodeRecursive(Graphics& gfx, Node* node);
    void CleanupDestroyedNodes(Node* currentNode);
    void ResetGame();
    void RemoveRigidbody(Node* currentNode);
    bool showDemoWindow = false;
    bool cursorEnabled = false;
    bool showControlWindow = false;
	bool freeViewCamera = false;
    bool paused = true;
    bool startedGame = false;
    float gameReset = 0;
    std::unique_ptr<Sprite> targetSprite;
    std::unique_ptr<Sprite> heart0Sprite; 
    std::unique_ptr<Sprite> heart1Sprite;
    std::unique_ptr<Sprite> heart2Sprite;
    std::unique_ptr<Sprite> heart3Sprite;

    std::unique_ptr<Sprite> mana0Sprite;
    std::unique_ptr<Sprite> mana1Sprite;
    std::unique_ptr<Sprite> mana2Sprite;
    std::unique_ptr<Sprite> mana3Sprite;

    std::unique_ptr<Sprite> hurtSprite;
    std::unique_ptr<Sprite> blackSprite;
    std::unique_ptr<Sprite> loadingScreen1;
    std::unique_ptr<Sprite> loadingScreen2;
    std::unique_ptr<Sprite> loadingScreen3;
    float countLoding = 0.0f;
    float bonusTime = 5.0f;

     
    bool wasMouseLeftPressedLastFrame = false;
     


    //========== pauza
    std::unique_ptr<Sprite> resumeBttn;  
    std::unique_ptr<Sprite> backBttn;
    std::unique_ptr<Sprite> pauseMenuBackground;
    void UpdatePauseMenu();
    void DrawPauseMenu(float dt);

    void DrawMainMenu(float dt);
    std::unique_ptr<Sprite> mainMenuBackground;
    std::unique_ptr<Sprite> quitBttn;
    std::unique_ptr<Sprite> startBttn;

    void LoadMainMenuResources();
    void UnloadMainMenuResources();
    void LoadPauseMenuResources();
    void UnloadPauseMenuResources();
};