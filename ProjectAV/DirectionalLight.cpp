#include "DirectionalLight.h"
#include "imgui/imgui.h"
#include "Camera.h"

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
	projWidth = 300.0f;
	projHeight = 300.0f;
	projNear = 1.0f;
	projFar = 1000.0f;
}

void DirectionalLight::Update(Graphics& gfx, const dx::XMFLOAT3& playerWorldPosition, Camera& playerCamera)  
{
	// 1. U¿yj pozycji gracza jako punktu centralnego (focus) dla frustum œwiat³a.
	dx::XMVECTOR lightTargetVec = dx::XMLoadFloat3(&playerWorldPosition);

	// 2. Za³aduj kierunek œwiat³a (zdefiniowany w przestrzeni œwiata) i znormalizuj go.
	dx::XMVECTOR lightDirectionVec = dx::XMVector3Normalize(dx::XMLoadFloat3(&worldDirection));

	// 3. Oblicz pozycjê "kamery" œwiat³a.
	//    Cofamy siê od pozycji gracza wzd³u¿ kierunku PRZECIWNEGO do padania œwiat³a.
	//    Odleg³oœæ cofniêcia: projFar / 2.0f jest dobrym punktem startowym.
	float lightCameraOffset = projFar / 2.0f;
	dx::XMVECTOR lightEyePositionVec = dx::XMVectorSubtract(lightTargetVec, dx::XMVectorScale(lightDirectionVec, lightCameraOffset));

	// 4. Zdefiniuj wektor "up" dla kamery œwiat³a.
	dx::XMVECTOR lightUpVec = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	if (abs(dx::XMVectorGetY(lightDirectionVec)) > 0.99f) // Jeœli kierunek jest prawie pionowy
	{
		lightUpVec = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // U¿yj innego "up"
	}

	// 5. Stwórz macierz widoku dla œwiat³a.
	viewMatrix = dx::XMMatrixLookAtLH(lightEyePositionVec, lightTargetVec, lightUpVec);

	// 6. Stwórz macierz projekcji ortogonalnej dla œwiat³a.
	//    Wymiary (projWidth, projHeight) i zakres (projNear, projFar)
	//    teraz definiuj¹ rozmiar "pude³ka cieni" wycentrowanego wokó³ pozycji gracza.
	projMatrix = dx::XMMatrixOrthographicLH(projWidth, projHeight, projNear, projFar);

	// 7. Przekszta³æ kierunek œwiat³a do przestrzeni widoku kamery gracza (dla shadera oœwietlenia).
	//    Nadal u¿ywamy playerCamera do tego, poniewa¿ shader oœwietlenia prawdopodobnie
	//    dzia³a w przestrzeni widoku kamery gracza.
	const dx::XMMATRIX playerViewMatrix = playerCamera.GetViewMatrix();
	dx::XMVECTOR viewSpaceLightDirection = dx::XMVector3TransformNormal(lightDirectionVec, playerViewMatrix);
	dx::XMStoreFloat3(&cbData.viewLightDirection, viewSpaceLightDirection);

	// Przypominam o miejscu aktualizacji buforów sta³ych (w Bind() lub tu¿ przed u¿yciem).
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