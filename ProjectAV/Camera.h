#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>

class Node; // Forward declare

class Camera : public Component
{
public:
	// Constructor takes owner (the camera position node), window, and the orientation node
	Camera(Node* owner, Window& window);
	virtual ~Camera() = default;

	virtual void Update(float dt) override;

	// Calculates and returns the View Matrix
	DirectX::XMMATRIX GetViewMatrix() const noexcept;

	// Optional: Set sensitivity
	void SetSensitivity(float xSens, float ySens) { sensX = xSens; sensY = ySens; }
	float active = false;
	float xRotation = 0.0f; // Pitch (applied to camera owner node)
	float yRotation = 0.0f; // Yaw (applied to orientation node)
private:
	void HandleMouseLook(float dt);

	Window& wnd;

	// Rotation state (stored internally, affects nodes)
	

	// Sensitivity
	float sensX = 0.1f; // Increased default sensitivity (like Unity example * Time.deltaTime)
	float sensY = 0.1f; // Increased default sensitivity

	// Pitch limits
	static constexpr float pitchLimit = DirectX::XM_PI / 2.0f * 0.99f;
};