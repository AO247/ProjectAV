#include "SpotLight.h"
#include "imgui/imgui.h"

SpotLight::SpotLight(Graphics& gfx, float radius)
    : mesh(gfx, radius),
    cbuf(gfx)
{
    Reset();
}

void SpotLight::SpawnControlWindow() noexcept
{
    if (ImGui::Begin("SpotLight"))
    {
        ImGui::Text("Position");
        ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f);
        ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f);
        ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f);

        ImGui::Text("Direction");
        if (ImGui::SliderFloat("Dir X", &cbData.dir.x, -1.0f, 1.0f)) {
			// Normalize the direction vector
            DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cbData.dir);
			dir = DirectX::XMVector3Normalize(dir);
            DirectX::XMStoreFloat3(&cbData.dir, dir);
        }
        if(ImGui::SliderFloat("Dir Y", &cbData.dir.y, -1.0f, 1.0f)) {
			// Normalize the direction vector
			DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cbData.dir);
			dir = DirectX::XMVector3Normalize(dir);
			DirectX::XMStoreFloat3(&cbData.dir, dir);
        }
        
        if(ImGui::SliderFloat("Dir Z", &cbData.dir.z, -1.0f, 1.0f)) {
            // Normalize the direction vector
            DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&cbData.dir);
            dir = DirectX::XMVector3Normalize(dir);
            DirectX::XMStoreFloat3(&cbData.dir, dir);
        }

        ImGui::Text("Intensity/Color");
        ImGui::ColorEdit3("Ambient", &cbData.ambient.x);
        ImGui::ColorEdit3("Diffuse Color", &cbData.diffuseColor.x);
        ImGui::SliderFloat("Intensity", &cbData.diffuseIntensity, 0.01f, 2.0f);

        ImGui::Text("Falloff");
        ImGui::SliderFloat("Constant", &cbData.attConst, 0.05f, 10.0f);
        ImGui::SliderFloat("Linear", &cbData.attLin, 0.0001f, 4.0f);
        ImGui::SliderFloat("Quadratic", &cbData.attQuad, 0.0000001f, 10.0f);

        ImGui::Text("Spotlight Properties");
        ImGui::SliderAngle("Cutoff Angle", &cbData.spotCutoff, 1.0f, 90.0f);
        ImGui::SliderFloat("Spot Exponent", &cbData.spotExponent, 1.0f, 128.0f);

        if (ImGui::Button("Reset"))
        {
            Reset();
        }
    }
    ImGui::End();
}

void SpotLight::Reset() noexcept
{
    cbData = {
        { 0.0f, 10.0f, -10.0f },  // Pozycja œwiat³a
        { 0.0f, -1.0f, 0.0f },   // Kierunek œwiat³a
        { 0.1f, 0.1f, 0.1f },    // Kolor ambient
        { 1.0f, 1.0f, 1.0f },    // Kolor œwiat³a rozproszonego
        1.0f,                    // Intensywnoœæ œwiat³a rozproszonego
        1.0f,                    // Sta³a sk³adowa t³umienia
        0.045f,                  // Liniowa sk³adowa t³umienia
        0.0075f,                 // Kwadratowa sk³adowa t³umienia
        DirectX::XMConvertToRadians(45.0f), // K¹t odciêcia sto¿ka (w radianach)
        10.0f                    // Wyg³adzenie krawêdzi sto¿ka
    };
}

void SpotLight::Draw(Graphics& gfx) noxnd
{
    mesh.SetPos(cbData.pos);
    mesh.Draw(gfx);
}

void SpotLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
    auto dataCopy = cbData;
    XMStoreFloat3(&dataCopy.pos, XMVector3Transform(XMLoadFloat3(&cbData.pos), view));
    XMStoreFloat3(&dataCopy.dir, XMVector3TransformNormal(XMLoadFloat3(&cbData.dir), view));
    cbuf.Update(gfx, dataCopy);
    cbuf.Bind(gfx);
}
