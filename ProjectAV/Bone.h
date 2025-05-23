// Bone.h
#pragma once

#include <DirectXMath.h> // For DirectX::XMFLOAT4X4 and DirectX::XMMatrixIdentity
#include <string>        // For std::string (though not strictly needed by BoneInfo itself yet, good to have for potential future extensions like bone names within BoneInfo)

struct BoneInfo
{
    int id;                           // Unique ID for the bone (this will be its index in the shader's bone matrix array)
    DirectX::XMFLOAT4X4 offsetMatrix; // This matrix transforms vertices from mesh space to the bone's local space.
    // It's also known as the inverse bind pose matrix for that bone.

// Default constructor
    BoneInfo() : id(-1) // Initialize id to an invalid state
    {
        // Initialize offsetMatrix to an identity matrix.
        // Assimp will provide the actual offset matrix during loading.
        DirectX::XMStoreFloat4x4(&offsetMatrix, DirectX::XMMatrixIdentity());
    }
};