#pragma once
#include <DirectXMath.h>

struct VectorKey {
    double time;
    DirectX::XMFLOAT3 value;
};

struct QuaternionKey {
    double time;
    DirectX::XMFLOAT4 value; // Store as XMFLOAT4 for DirectX::XMMatrixRotationQuaternion
};



class Keyframe
{
};

