#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
// #include "Camera.h" // Remove Camera include for now
#include "PointLight.h"
#include <memory> 
#include "Node.h" 
#include "PhysicsEngine.h"
#include <map>
#include "SolidBox.h"
#include "Components.h"
// Forward declarations
class PlayerController; // Forward declare

class App
{
public:
    App();
    App(const std::string& commandLine = "");
    int Go();
    ~App();
private:
    void DoFrame(float dt);
    void HandleInput(float dt);
    void ShowControlWindows();

private:
    // Core App Systems
    std::string commandLine;
    ImguiManager imgui;
    Window wnd; // PlayerController needs access to this
    Timer timer;
    float speed_factor = 1.0f;
    PointLight pointLight;
    PhysicsEngine physicsEngine; // Physics engine instance

    // --- Scene Graph ---
    std::unique_ptr<Node> pSceneRoot;
    Node* pPlayerNode = nullptr; // Node representing the player capsule/origin
    // --- Pointers to other nodes (keep as is) ---
    Node* pNanosuitNode = nullptr;
    Node* pNanosuitNode2 = nullptr;
    Node* pBox = nullptr;
    Node* pEmptyNode = nullptr;
    Node* pBrick = nullptr;
    Node* pStone = nullptr;
    Node* pColumn = nullptr;
    Node* pIsland = nullptr;
    Node* pSelectedSceneNode = nullptr;
    Node* pNoxTurn = nullptr; // Node for the camera
    Node* pNoxTurnHair = nullptr; // Node for the camera
    Node* pTestModel = nullptr; // Node for the test model
    Node* pEnemy = nullptr; // Node for the enemy
    // --- UI State ---

    std::map<BoundingSphere*, SolidSphere> sphereCollidersToDraw;
    std::map<OBB*, SolidBox> boxCollidersToDraw; 
    void AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere);
    void DrawSphereColliders(Graphics& gfx);
    void AddBoxColliderToDraw(Graphics& gfx, OBB* obb);
    void DrawBoxColliders(Graphics& gfx);

    bool showDemoWindow = false;
    bool cursorEnabled = false;
    bool showControlWindow = true;
};