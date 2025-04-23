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
        alignas(16) DirectX::XMFLOAT3 pos;          // Pozycja �wiat�a
        alignas(16) DirectX::XMFLOAT3 dir;          // Kierunek �wiat�a
        alignas(16) DirectX::XMFLOAT3 ambient;      // Kolor ambient
        alignas(16) DirectX::XMFLOAT3 diffuseColor; // Kolor �wiat�a rozproszonego
        float diffuseIntensity;                     // Intensywno�� �wiat�a rozproszonego
        float attConst;                             // Sta�a sk�adowa t�umienia
        float attLin;                               // Liniowa sk�adowa t�umienia
        float attQuad;                              // Kwadratowa sk�adowa t�umienia
        float spotCutoff;                           // K�t odci�cia sto�ka (w radianach)
        float spotExponent;                         // Wyg�adzenie kraw�dzi sto�ka
    };

private:
    SpotLightCBuf cbData; // Dane bufora sta�ego
    mutable Bind::PixelConstantBuffer<SpotLightCBuf> cbuf; // Bufor sta�y
    SolidSphere mesh; // Siatka reprezentuj�ca �wiat�o
};
