#include "DirectionalLight.h"
#include "imgui/imgui.h"

DirectionalLight::DirectionalLight(Graphics& gfx, UINT slot)
	:
	cbuf(gfx, slot)
{
	Reset();
}

void DirectionalLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Directional Light"))
	{
		ImGui::Text("Direction");
		ImGui::SliderFloat3("##Direction", &cbData.direction.x, -1.0f, 1.0f, "%.2f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.0f, 5.0f, "%.2f");
		ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
		ImGui::ColorEdit3("Ambient", &cbData.ambient.x);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void DirectionalLight::Reset() noexcept
{
	cbData = {
		{ -0.5f, -0.8f, 0.2f }, 
		1.0f,
		{ 0.5f, 0.5f, 0.5f },
		1.0f,
		{ 1.0f, 1.0f, 1.0f },
		1.0f
	};
}

void DirectionalLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{

	DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cbData.direction);
	dir = DirectX::XMVector3Normalize(dir);

	auto dataCopy = cbData;

	const auto transformedDir = DirectX::XMVector3TransformNormal(dir, view);
	DirectX::XMStoreFloat3(&dataCopy.direction, transformedDir);

	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}