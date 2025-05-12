#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include "Keyframe.h"

struct BoneInfo {
    int id; // Index in the final bone matrices array
    std::string name;
    DirectX::XMFLOAT4X4 offsetMatrix; // Inverse Bind Pose Matrix
    // You might also store parent index, children indices for easy traversal
};


struct BoneAnimation {
    std::string boneName;
    std::vector<VectorKey> positionKeys;
    std::vector<QuaternionKey> rotationKeys;
    std::vector<VectorKey> scaleKeys;

    // Helper to interpolate
    DirectX::XMFLOAT4X4 Interpolate(float animationTime);
};
class Bone
{
};

