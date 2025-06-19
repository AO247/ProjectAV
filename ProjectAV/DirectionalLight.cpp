#include "DirectionalLight.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

DirectionalLight::DirectionalLight(Graphics& gfx, UINT slot)
	: cbuf(gfx, slot)
{
	Reset();
}

void DirectionalLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Directional Light"))
	{
		ImGui::Text("Direction");
		// ZMIANA: Kontrolujemy teraz worldDirection
		ImGui::SliderFloat3("##Direction", &worldDirection.x, -1.0f, 1.0f, "%.2f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.0f, 5.0f, "%.2f");
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		ImGui::Separator();
		ImGui::Text("Shadow Projection");
		ImGui::SliderFloat("Width", &projWidth, 1.0f, 800.0f, "%.1f");
		ImGui::SliderFloat("Height", &projHeight, 1.0f, 800.0f, "%.1f");
		ImGui::SliderFloat("Near Z", &projNear, 0.1f, 100.0f, "%.1f");
		ImGui::SliderFloat("Far Z", &projFar, 1.0f, 2000.0f, "%.1f");

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void DirectionalLight::Reset() noexcept
{
	// ZMIANA: Inicjalizujemy osobno worldDirection i cbData
	worldDirection = { -0.5f, -0.8f, 0.2f };
	cbData = {
		{0.0f, 0.0f, 0.0f}, 0.0f,   // viewLightDirection (obliczane co klatkê)
		{ 0.15f, 0.15f, 0.15f }, 0.0f, // Ambient
		{ 1.0f, 1.0f, 1.0f },    // Diffuse Color
		1.0f,                    // Diffuse Intensity
	};
	projWidth = 400.0f;
	projHeight = 400.0f;
	projNear = 1.0f;
	projFar = 1000.0f;
}

void DirectionalLight::Update(Graphics& gfx, const dx::XMFLOAT3& lookAt) noexcept
{
	// ZMIANA: U¿ywamy teraz `worldDirection`
	dx::XMVECTOR dir = dx::XMVector3Normalize(dx::XMLoadFloat3(&worldDirection));

	dx::XMVECTOR eyePos = dx::XMVectorSubtract(dx::XMLoadFloat3(&lookAt), dx::XMVectorScale(dir, projFar / 2.0f));
	viewMatrix = dx::XMMatrixLookAtLH(eyePos, dx::XMLoadFloat3(&lookAt), dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	projMatrix = dx::XMMatrixOrthographicLH(projWidth, projHeight, projNear, projFar);

	// Przekszta³æ kierunek œwiat³a do przestrzeni widoku kamery gracza
	const auto viewLightDir = dx::XMVector3TransformNormal(dir, gfx.GetCamera());
	// Zapisz go w buforze, który wyœlemy do GPU
	dx::XMStoreFloat3(&cbData.viewLightDirection, viewLightDir);
}

void DirectionalLight::Bind(Graphics& gfx) const noexcept
{
	// Ta funkcja jest teraz idealnie czysta - po prostu wysy³a gotowe dane.
	cbuf.Update(gfx, cbData);
	cbuf.Bind(gfx);
}

dx::XMMATRIX DirectionalLight::GetViewMatrix() const noexcept
{
	return viewMatrix;
}

dx::XMMATRIX DirectionalLight::GetProjectionMatrix() const noexcept
{
	return projMatrix;
}