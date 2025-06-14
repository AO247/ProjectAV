#pragma once

#include <DirectXMath.h>
#include <assimp/anim.h>

inline DirectX::XMFLOAT3 GetDXVec(const aiVector3D& vec) {
    return DirectX::XMFLOAT3(vec.x, vec.y, vec.z);
}

inline DirectX::XMFLOAT4 GetDXQuat(const aiQuaternion& quat)
{
    return DirectX::XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
}

inline DirectX::XMMATRIX GetDXMat(const aiMatrix4x4& mat) {
    return DirectX::XMMATRIX(
        mat.a1, mat.a2, mat.a3, mat.a4,
        mat.b1, mat.b2, mat.b3, mat.b4,
        mat.c1, mat.c2, mat.c3, mat.c4,
        mat.d1, mat.d2, mat.d3, mat.d4
    );
}