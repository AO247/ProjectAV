#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include <memory>
#include "Node.h"
#include "PhysicsEngine.h"
#include <map>
#include "Components.h"
#include "ScriptCommander.h"
#include "FrameCommander.h"
#include "ColliderSphere.h"
#include "SolidBox.h"
#include "SolidCapsule.h"
#include "Button.h"  
#include "AnimatedSprite.h"
#include "Sprite.h"
// Forward declarations
class PlayerController; // Forward declare

class App
{
public:
    App();
    App(const std::string& commandLine = "");
    int Go();
    ~App();
    void OnMouseClick(int xPos, int yPos);
private:
    void DoFrame(float dt);
    void HandleInput(float dt);
    void ShowControlWindows();

private:
    // Core App Systems
    std::string commandLine;
    ImguiManager imgui;
    Window wnd; // PlayerController needs access to this
    ScriptCommander scriptCommander;
    Timer timer;
    float speed_factor = 1.0f;
    PointLight pointLight;
    PhysicsEngine physicsEngine; // Physics engine instance
    DirectX::BoundingFrustum cameraFrustum; // Frustum for the camera
    FrameCommander fc;

    // --- Scene Graph ---
    std::unique_ptr<Node> pSceneRoot;
    Node* pCamera = nullptr;
    Node* pFreeViewCamera = nullptr; // Node for the free view camera
    Node* pPlayer = nullptr; // Node representing the player capsule/origin
    Node* pAbility1 = nullptr; // Node for the first ability
    Node* pAbility2 = nullptr; // Node for the second ability
    // --- Pointers to other nodes (keep as is) ---
    Node* pNanosuitNode = nullptr;
    Node* pBox = nullptr;
    Node* pEmptyNode = nullptr;
    Node* pBrick = nullptr;
    Node* pStone = nullptr;
    Node* pColumn = nullptr;
    Node* pColumn2 = nullptr;
    Node* pColumn3 = nullptr;
    Node* pColumn4 = nullptr;
    Node* pIsland = nullptr;
    Node* pSelectedSceneNode = nullptr;
    Node* pNoxTurn = nullptr; // Node for the camera
    Node* pNoxTurnHair = nullptr; // Node for the camera
    Node* pEnemy = nullptr; // Node for the enemy

    // --- UI State ---

    std::map<BoundingSphere*, ColliderSphere*> sphereCollidersToDraw;

    std::map<OBB*, SolidBox*> boxCollidersToDraw;
    std::map<CapsuleCollider*, SolidCapsule*> capsuleCollidersToDraw;

    void AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere);
    void DrawSphereColliders(Graphics& gfx);
    void AddBoxColliderToDraw(Graphics& gfx, OBB* obb);
    void DrawBoxColliders(Graphics& gfx);
    void AddCapsuleColliderToDraw(Graphics& gfx, CapsuleCollider* capsule);
    void DrawCapsuleColliders(Graphics& gfx);
    void FrustumCalculating();
    void DrawNodeRecursive(Graphics& gfx, Node& node);


    bool showDemoWindow = false;
    bool cursorEnabled = false;
    bool showControlWindow = true;
    bool freeViewCamera = false;

    std::unique_ptr<Button> myButton;
    std::unique_ptr<AnimatedSprite> mySprite;
    std::unique_ptr<Sprite> myPlusSprite;
};