#pragma once

#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>

class Node;

class Camera : public Component
{
public:
	Camera(Node* owner, Window& window);
	virtual ~Camera() = default;

	virtual void Update(float dt) override;

	DirectX::XMMATRIX GetViewMatrix() const noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;

	void SetSensitivity(float xSens, float ySens) { sensX = xSens; sensY = ySens; }
	float active = false;
	float xRotation = 0.0f;
	float yRotation = 0.0f;
private:
	void HandleMouseLook(float dt);

	Window& wnd;

	

	float sensX = 0.1f; 
	float sensY = 0.1f; 

	static constexpr float pitchLimit = DirectX::XM_PI / 2.0f * 0.99f;
};