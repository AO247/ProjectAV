#pragma once
#include <DirectXMath.h>

struct ParticleCbuf
{
    DirectX::XMMATRIX viewProjection;
    DirectX::XMFLOAT3 cameraPosition;
    // Padding to make the struct size a multiple of 16
    float padding;
};