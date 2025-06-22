// In ParticleCbuf.h

#pragma once
#include <DirectXMath.h>

struct ParticleCbuf
{
    DirectX::XMMATRIX viewProjection;
    DirectX::XMFLOAT3 cameraPosition;
    float padding;
    int lockY;

    // +++ ADD THIS NEW MEMBER +++
    DirectX::XMFLOAT2 atlasSize; // Stores {1/cols, 1/rows}
    float padding2[1]; // Adjust padding
};