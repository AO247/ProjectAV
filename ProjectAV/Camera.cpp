#include "Camera.h" // Include the header for this class
#include "Node.h"            // Include Node for GetOwner/SetLocalRotation
#include "Window.h"          // For wnd access
#include "CMath.h"           // For wrap_angle, PI
#include <DirectXMath.h>
#include <algorithm>        // For std::clamp
#include "Graphics.h"        // For Graphics

namespace dx = DirectX;

Camera::Camera(Node* owner, Window& window, Graphics& gfx, std::string name, DirectX::XMFLOAT3 homePos, float homePitch, float homeYaw, bool tethered) noexcept
    : Component(owner),
    wnd(window),
	name(std::move(name)),
	homePos(homePos),
	homePitch(homePitch),
	homeYaw(homeYaw),
	proj(gfx, 1.0f, 9.0f / 16.0f, 0.5f, 400.0f),
	indicator(gfx),
	tethered(tethered)
{
	if (tethered)
	{
		pos = homePos;
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
	Reset(gfx);
}

void Camera::Update(float dt)
{
    // Don't process look if cursor is enabled OR if required nodes are missing
    if (!wnd.CursorEnabled() && GetOwner() && active && !wnd.playerLocked)
    {
        HandleMouseLook(dt);
    }
}

void Camera::HandleMouseLook(float dt)
{
    float mouseX = 0.0f;
    float mouseY = 0.0f;

    // Accumulate mouse delta for this frame
    while (const auto delta = wnd.mouse.ReadRawDelta())
    {
        mouseX += delta->x;
        mouseY -= delta->y;
    }

    yRotation += mouseX * sensX * 0.01f; // Apply Yaw delta (adjust multiplier)
    xRotation -= mouseY * sensY * 0.01f; // Apply Pitch delta (inverted Y, adjust multiplier)

    // Clamp pitch
    xRotation = std::clamp(xRotation, -pitchLimit, pitchLimit);
    // Wrap Yaw (optional, but good practice)
    yRotation = wrap_angle(yRotation);
    DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0.0f);
    DirectX::XMFLOAT4 quatFloat4;
    DirectX::XMStoreFloat4(&quatFloat4, quat);

    GetOwner()->SetLocalRotation(quatFloat4);      // Apply Pitch to camera node's X axis
}


DirectX::XMMATRIX Camera::GetViewMatrix() const noexcept
{
    dx::XMMATRIX cameraWorldTransform = GetOwner()->GetWorldTransform();

    dx::XMVECTOR determinant = dx::XMMatrixDeterminant(cameraWorldTransform);
    return dx::XMMatrixInverse(&determinant, cameraWorldTransform);
}

void Camera::BindToGraphics(Graphics& gfx) const
{
	gfx.SetCamera(GetMatrix());
	gfx.SetProjection(proj.GetMatrix());
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform(forwardBaseVector,
		XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3(&pos);
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH(camPosition, camTarget, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return proj.GetMatrix();
}

void Camera::SpawnControlWidgets(Graphics& gfx) noexcept
{
	bool rotDirty = false;
	bool posDirty = false;
	const auto dcheck = [](bool d, bool& carry) { carry = carry || d; };
	if (!tethered)
	{
		ImGui::Text("Position");
		dcheck(ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f"), posDirty);
		dcheck(ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f"), posDirty);
	}
	ImGui::Text("Orientation");
	dcheck(ImGui::SliderAngle("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty);
	dcheck(ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f), rotDirty);
	proj.RenderWidgets(gfx);
	ImGui::Checkbox("Camera Indicator", &enableCameraIndicator);
	ImGui::Checkbox("Frustum Indicator", &enableFrustumIndicator);
	if (ImGui::Button("Reset"))
	{
		Reset(gfx);
	}

	if (rotDirty)
	{
		const dx::XMFLOAT3 angles = { pitch,yaw,0.0f };
		indicator.SetRotation(angles);
		proj.SetRotation(angles);
	}
	if (posDirty)
	{
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
}

void Camera::Reset(Graphics& gfx) noexcept
{
	if (!tethered)
	{
		pos = homePos;
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
	pitch = homePitch;
	yaw = homeYaw;

	const dx::XMFLOAT3 angles = { pitch,yaw,0.0f };
	indicator.SetRotation(angles);
	proj.SetRotation(angles);
	proj.Reset(gfx);
}

void Camera::Rotate(float dx, float dy) noexcept
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2.0f, 0.995f * PI / 2.0f);
	const dx::XMFLOAT3 angles = { pitch,yaw,0.0f };
	indicator.SetRotation(angles);
	proj.SetRotation(angles);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	if (!tethered)
	{
		dx::XMStoreFloat3(&translation, dx::XMVector3Transform(
			dx::XMLoadFloat3(&translation),
			dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) *
			dx::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
		));
		pos = {
			pos.x + translation.x,
			pos.y + translation.y,
			pos.z + translation.z
		};
		indicator.SetPos(pos);
		proj.SetPos(pos);
	}
}

DirectX::XMFLOAT3 Camera::GetPos() const noexcept
{
	return pos;
}

void Camera::SetPos(const DirectX::XMFLOAT3& pos) noexcept
{
	this->pos = pos;
	indicator.SetPos(pos);
	proj.SetPos(pos);
}

const std::string& Camera::GetName() const noexcept
{
	return name;
}

void Camera::LinkTechniques(Rgph::RenderGraph& rg)
{
	indicator.LinkTechniques(rg);
	proj.LinkTechniques(rg);
}

void Camera::Submit(size_t channels) const
{
	if (enableCameraIndicator)
	{
		indicator.Submit(channels);
	}
	if (enableFrustumIndicator)
	{
		proj.Submit(channels);
	}
}