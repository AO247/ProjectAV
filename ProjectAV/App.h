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
#include "MusicBuffer.h"
#include "SoundDevice.h"
#include "ScriptCommander.h"
#include "FrameCommander.h"
#include "ColliderSphere.h"
#include "SolidBox.h"
#include "SolidCapsule.h"
#include "DebugLine.h"
#include "PhysicsCommon.h"
#include "TestCube.h"
#include "Global.h"
#include "Sprite.h"

// Forward declarations
class PlayerController; // Forward declare

class App
{
public:
    App(const std::string& commandLine = "");
    int Go();
    ~App();
private:
    void DoFrame(float dt);
    void HandleInput(float dt);
    void ShowControlWindows();
    void ForEnemyWalking();

private:
    // Core App Systems
    std::string commandLine;
    ImguiManager imgui;
    Window wnd; // PlayerController needs access to this
    ScriptCommander scriptCommander;
    Timer timer;
    float speed_factor = 1.0f;
    PointLight pointLight;
    //PhysicsEngine physicsEngine; // Physics engine instance


    PhysicsSystem* physicsSystem;
    TempAllocatorImpl* temp_allocator;
    JobSystemThreadPool* job_system;
    MyContactListener* contactListener;


    SoundDevice* soundDevice; // Sound device instance
    std::unique_ptr<MusicBuffer> myMusic;
    DirectX::BoundingFrustum cameraFrustum; // Frustum for the camera
    FrameCommander fc{ wnd.Gfx() };

    DebugLine* line1 = new DebugLine(wnd.Gfx(), {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, { 1.0f, 0.0f, 0.0f, 1.0f });    
    DebugLine* line2 = new DebugLine(wnd.Gfx(), { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
    DebugLine* line3 = new DebugLine(wnd.Gfx(), { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });
    DebugLine* line4 = new DebugLine(wnd.Gfx(), { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f });

    // --- Scene Graph ---
    std::unique_ptr<Node> pSceneRoot;
    Node* pCamera = nullptr;
	Node* pFreeViewCamera = nullptr; // Node for the free view camera
    Node* pPlayer = nullptr; // Node representing the player capsule/origin
	Node* pAbility1 = nullptr; // Node for the first ability
	Node* pAbility2 = nullptr; // Node for the second ability
	Node* pAbility3 = nullptr; // Node for the third ability
    Node* pAbility4 = nullptr;
	Node* pSelectedSceneNode = nullptr; // Node representing the selected scene node
	Node* pSoundEffectsPlayer = nullptr; // Node for the sound effects player
    Node* pPrefabs = nullptr;
	Node* pHands = nullptr;

    // --- UI State ---
    
    /*std::map<BoundingSphere*, ColliderSphere*> sphereCollidersToDraw;

    std::map<OBB*, SolidBox*> boxCollidersToDraw;
	std::map<CapsuleCollider*, SolidCapsule*> capsuleCollidersToDraw;*/

    /*void AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere);
    void DrawSphereColliders(Graphics& gfx);
    void AddBoxColliderToDraw(Graphics& gfx, OBB* obb);
    void DrawBoxColliders(Graphics& gfx);
	void AddCapsuleColliderToDraw(Graphics& gfx, CapsuleCollider* capsule);*/
	//void DrawCapsuleColliders(Graphics& gfx);
    void FrustumCalculating();
    void DrawNodeRecursive(Graphics& gfx, Node& node);
    void CleanupDestroyedNodes(Node* currentNode);

    bool showDemoWindow = false;
    bool cursorEnabled = false;
    bool showControlWindow = false;
	bool freeViewCamera = false;


    //===========UI===========

    std::unique_ptr<Sprite> targetSprite;
    std::unique_ptr<Sprite> heart1Sprite;
    std::unique_ptr<Sprite> heart2Sprite;
    std::unique_ptr<Sprite> heart3Sprite;
};