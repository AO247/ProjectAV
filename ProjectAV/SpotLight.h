#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "BindableCommon.h"
#include <DirectXMath.h>

class SpotLight
{
public:
    SpotLight(Graphics& gfx, float radius = 0.5f);
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
    void Draw(Graphics& gfx) noxnd;
    void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;

private:
    struct SpotLightCBuf
    {
        alignas(16) DirectX::XMFLOAT3 pos;          // Pozycja œwiat³a
        alignas(16) DirectX::XMFLOAT3 dir;          // Kierunek œwiat³a
        alignas(16) DirectX::XMFLOAT3 ambient;      // Kolor ambient
        alignas(16) DirectX::XMFLOAT3 diffuseColor; // Kolor œwiat³a rozproszonego
        float diffuseIntensity;                     // Intensywnoœæ œwiat³a rozproszonego
        float attConst;                             // Sta³a sk³adowa t³umienia
        float attLin;                               // Liniowa sk³adowa t³umienia
        float attQuad;                              // Kwadratowa sk³adowa t³umienia
        float spotCutoff;                           // K¹t odciêcia sto¿ka (w radianach)
        float spotExponent;                         // Wyg³adzenie krawêdzi sto¿ka
    };

private:
    SpotLightCBuf cbData; // Dane bufora sta³ego
    mutable Bind::PixelConstantBuffer<SpotLightCBuf> cbuf; // Bufor sta³y
    SolidSphere mesh; // Siatka reprezentuj¹ca œwiat³o
};
