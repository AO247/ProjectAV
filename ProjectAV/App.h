#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include <memory> 
#include "Node.h" 
#include "PhysicsEngine.h"
#include <map>
#include "SolidBox.h"
#include "Components.h"
#include "MusicBuffer.h"
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
    std::unique_ptr<MusicBuffer> myMusic;

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
	Node* pSoundEffectsPlayer = nullptr; // Node for the sound effects player
    // --- UI State ---

    std::map<BoundingSphere*, SolidSphere> sphereCollidersToDraw;
    std::map<OBB*, SolidSphere> boxCollidersToDraw; 
	std::map<CapsuleCollider*, SolidSphere> capsuleCollidersToDraw;

    void AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere);
    void DrawSphereColliders(Graphics& gfx);
    void AddBoxColliderToDraw(Graphics& gfx, OBB* obb);
    void DrawBoxColliders(Graphics& gfx);
	void AddCapsuleColliderToDraw(Graphics& gfx, CapsuleCollider* capsule);
	void DrawCapsuleColliders(Graphics& gfx);

    bool showDemoWindow = false;
    bool cursorEnabled = false;
    bool showControlWindow = true;
	bool freeViewCamera = false;
};