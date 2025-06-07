#pragma once

#include "Component.h"
#include "Window.h" // Needs access to Window for input
#include <DirectXMath.h>
#include <string>
#include "Projection.h"
#include "CameraIndicator.h"

class Graphics;
namespace Rgph
{
	class RenderGraph;
}

class Node; // Forward declare

class Camera : public Component
{
public:
	Camera(Node* owner, Window& window, Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos = { 0.0f,0.0f,0.0f }, float homePitch = 0.0f, float homeYaw = 0.0f, bool tethered = false) noexcept;
	virtual ~Camera() = default;

	virtual void Update(float dt) override;

	DirectX::XMMATRIX GetViewMatrix() const noexcept;

	void SetSensitivity(float xSens, float ySens) { sensX = xSens; sensY = ySens; }
	float active = false;
	float xRotation = 0.0f; 
	float yRotation = 0.0f; 

	void BindToGraphics(Graphics& gfx) const;
	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SpawnControlWidgets(Graphics& gfx) noexcept;
	void Reset(Graphics& gfx) noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques(Rgph::RenderGraph& rg);
	void Submit(size_t channel) const;
private:
	void HandleMouseLook(float dt);

	Window& wnd;

	// Rotation state (stored internally, affects nodes)
	bool tethered;
	std::string name;
	DirectX::XMFLOAT3 homePos;
	float homePitch;
	float homeYaw;
	DirectX::XMFLOAT3 pos;
	float pitch;
	float yaw;
	static constexpr float travelSpeed = 12.0f;
	static constexpr float rotationSpeed = 0.004f;
	bool enableCameraIndicator = true;
	bool enableFrustumIndicator = true;
	Projection proj;
	CameraIndicator indicator;

	// Sensitivity
	float sensX = 0.1f; // Increased default sensitivity (like Unity example * Time.deltaTime)
	float sensY = 0.1f; // Increased default sensitivity

	// Pitch limits
	static constexpr float pitchLimit = DirectX::XM_PI / 2.0f * 0.99f;
};