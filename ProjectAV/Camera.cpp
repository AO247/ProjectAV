#include "Camera.h"
#include "imgui/imgui.h"
#include "CMath.h"

namespace dx = DirectX;

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace dx;

	const dx::XMVECTOR forwardBaseVector = XMVectorSet( 0.0f,0.0f,1.0f,0.0f );
	// apply the camera rotations to a base vector
	const auto lookVector = XMVector3Transform( forwardBaseVector,
		XMMatrixRotationRollPitchYaw( pitch,yaw,0.0f )
	);
	// generate camera transform (applied to all objects to arrange them relative
	// to camera position/orientation in world) from cam position and direction
	// camera "top" always faces towards +Y (cannot do a barrel roll)
	const auto camPosition = XMLoadFloat3( &pos );
	const auto camTarget = camPosition + lookVector;
	return XMMatrixLookAtLH( camPosition,camTarget,XMVectorSet( 0.0f,1.0f,0.0f,0.0f ) );
}

void Camera::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Camera" ) )
	{
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-80.0f,80.0f);
		ImGui::SliderFloat( "Y",&pos.y,-80.0f,80.0f);
		ImGui::SliderFloat( "Z",&pos.z,-80.0f,80.0f);
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Pitch",&pitch,0.995f * -90.0f,0.995f * 90.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos = { 0.0f,7.5f,-18.0f };
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::Rotate( float dx,float dy ) noexcept
{
	yaw = wrap_angle( yaw + dx * rotationSpeed );
	pitch = std::clamp( pitch + dy * rotationSpeed,0.995f * -PI / 2.0f,0.995f * PI / 2.0f );
}

void Camera::Translate( DirectX::XMFLOAT3 translation ) noexcept
{
	dx::XMStoreFloat3( &translation,dx::XMVector3Transform(
		dx::XMLoadFloat3( &translation ),
		dx::XMMatrixRotationRollPitchYaw( pitch,yaw,0.0f ) *
		dx::XMMatrixScaling( travelSpeed,travelSpeed,travelSpeed )
	) );
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z
	};
}
DirectX::XMMATRIX Camera::GetMatrix(DirectX::FXMMATRIX targetWorldTransform,
	DirectX::XMFLOAT3 eyeOffset) const noexcept
{
	// Define base vectors
	const dx::XMVECTOR forwardVector = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // Player looks down +Z locally
	const dx::XMVECTOR upVector = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);    // Player's up is +Y locally

	// Extract position (eye position) from the target's world matrix
	dx::XMVECTOR eyePosition = targetWorldTransform.r[3];

	// Apply offset if provided (transform offset into world space using target's rotation)
	if (eyeOffset.x != 0.0f || eyeOffset.y != 0.0f || eyeOffset.z != 0.0f) {
		dx::XMMATRIX rotationOnly = targetWorldTransform;
		rotationOnly.r[3] = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Zero out translation
		dx::XMVECTOR offsetVec = dx::XMLoadFloat3(&eyeOffset);
		offsetVec = dx::XMVector3TransformNormal(offsetVec, rotationOnly); // Rotate offset
		eyePosition = dx::XMVectorAdd(eyePosition, offsetVec); // Add world-space offset
	}

	// Transform the local forward and up vectors into world space using the target's orientation
	// Use TransformNormal as these are directions, not positions
	const dx::XMVECTOR lookVectorWorld = dx::XMVector3TransformNormal(forwardVector, targetWorldTransform);
	const dx::XMVECTOR upVectorWorld = dx::XMVector3TransformNormal(upVector, targetWorldTransform);

	// Calculate the look-at target position in world space
	const dx::XMVECTOR lookAtTarget = dx::XMVectorAdd(eyePosition, lookVectorWorld);

	// Build the final view matrix
	return dx::XMMatrixLookAtLH(eyePosition, lookAtTarget, upVectorWorld);
}