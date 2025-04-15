#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <memory> // For unique_ptr
#include "Node.h" // Include the new Node class
#include "PhysicsEngine.h"
#include <map>
#include "BoundingSphere.h"
#include "SolidBox.h"

// Forward declarations (if needed)


class App
{
public:
    App();
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
    PhysicsEngine physicsEngine;

    // --- Scene Graph ---
    std::unique_ptr<Node> pSceneRoot; // Root of the scene graph
    Node* pNanosuitNode = nullptr;    // Pointer to the specific node holding the nanosuit model (optional, for easy access)
    Node* pNanosuitNode2 = nullptr;    // Pointer to the specific node holding the nanosuit model (optional, for easy access)
    Node* pBox = nullptr;    // Pointer to the specific node holding the nanosuit model (optional, for easy access)
    Node* pEmptyNode = nullptr;
    Node* pSelectedSceneNode = nullptr; // Add this pointer

    std::map<BoundingSphere*, SolidSphere> sphereCollidersToDraw;
    std::map<AxisAligned::AABB*, SolidBox> boxCollidersToDraw;
    void AddSphereColliderToDraw(Graphics& gfx, BoundingSphere* boundingSphere);
    void DrawSphereColliders(Graphics& gfx);
    void AddBoxColliderToDraw(Graphics& gfx, AxisAligned::AABB* aabb);
    void DrawBoxColliders(Graphics& gfx);

    // --- UI State ---
    bool showDemoWindow = false;
    bool cursorEnabled = false; // Track cursor state explicitly
	bool showControlWindow = true; // Show control window by default
};