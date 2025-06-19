// In ParticleCbuf.h

#pragma once
#include <DirectXMath.h>

// NO <windows.h> include needed!

struct ParticleCbuf
{
    DirectX::XMMATRIX viewProjection;
    DirectX::XMFLOAT3 cameraPosition;
    // Padding to make the struct size a multiple of 16
    float padding;

    // +++ CHANGE THIS LINE FROM BOOL to int +++
    int lockY; // The flag to control billboarding mode in the shader
    // More padding to keep the total size a multiple of 16
    float padding2[3];
};