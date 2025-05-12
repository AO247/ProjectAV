#pragma once

#include <string>
#include <vector>
#include <map>
#include <DirectXMath.h>
#include <algorithm> // For std.find_if, std.min, std::max
#include "Keyframe.h"

// Maximum number of bones supported by the shader
const int MAX_BONES_PER_MESH = 64; // Or whatever your shader supports

struct BoneAnimationChannel {
    std::string boneName;
    std::vector<VectorKey> positionKeys;
    std::vector<QuaternionKey> rotationKeys;
    std::vector<VectorKey> scaleKeys;

    // Helper to find the index of the keyframe at or before the given animation time
    template<typename T>
    int GetKeyIndex(float animationTime, const std::vector<T>& keys) const {
        if (keys.empty()) return -1;
        for (int index = 0; index < static_cast<int>(keys.size()) - 1; ++index) {
            if (animationTime < keys[index + 1].time)
                return index;
        }
        return static_cast<int>(keys.size()) - 1;
    }

    DirectX::XMVECTOR InterpolatePosition(float animationTime) const {
        if (positionKeys.empty()) return DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        if (positionKeys.size() == 1) return DirectX::XMLoadFloat3(&positionKeys[0].value);

        int p0Index = GetKeyIndex(animationTime, positionKeys);
        int p1Index = p0Index + 1;
        if (p1Index >= positionKeys.size()) return DirectX::XMLoadFloat3(&positionKeys.back().value);

        float t0 = positionKeys[p0Index].time;
        float t1 = positionKeys[p1Index].time;
        float factor = (t1 - t0 < 1e-6f) ? 0.0f : (animationTime - t0) / (t1 - t0);
        factor = std::max(0.0f, std::min(1.0f, factor));

        return DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&positionKeys[p0Index].value), DirectX::XMLoadFloat3(&positionKeys[p1Index].value), factor);
    }

    DirectX::XMVECTOR InterpolateRotation(float animationTime) const {
        if (rotationKeys.empty()) return DirectX::XMQuaternionIdentity();
        if (rotationKeys.size() == 1) return DirectX::XMLoadFloat4(&rotationKeys[0].value);

        int r0Index = GetKeyIndex(animationTime, rotationKeys);
        int r1Index = r0Index + 1;
        if (r1Index >= rotationKeys.size()) return DirectX::XMLoadFloat4(&rotationKeys.back().value);

        float t0 = rotationKeys[r0Index].time;
        float t1 = rotationKeys[r1Index].time;
        float factor = (t1 - t0 < 1e-6f) ? 0.0f : (animationTime - t0) / (t1 - t0);
        factor = std::max(0.0f, std::min(1.0f, factor));

        return DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&rotationKeys[r0Index].value), DirectX::XMLoadFloat4(&rotationKeys[r1Index].value), factor);
    }

    DirectX::XMVECTOR InterpolateScale(float animationTime) const {
        if (scaleKeys.empty()) return DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f);
        if (scaleKeys.size() == 1) return DirectX::XMLoadFloat3(&scaleKeys[0].value);

        int s0Index = GetKeyIndex(animationTime, scaleKeys);
        int s1Index = s0Index + 1;
        if (s1Index >= scaleKeys.size()) return DirectX::XMLoadFloat3(&scaleKeys.back().value);

        float t0 = scaleKeys[s0Index].time;
        float t1 = scaleKeys[s1Index].time;
        float factor = (t1 - t0 < 1e-6f) ? 0.0f : (animationTime - t0) / (t1 - t0);
        factor = std::max(0.0f, std::min(1.0f, factor));

        return DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&scaleKeys[s0Index].value), DirectX::XMLoadFloat3(&scaleKeys[s1Index].value), factor);
    }

    DirectX::XMMATRIX GetInterpolatedTransform(float animationTime) const {
        DirectX::XMVECTOR S_vec = InterpolateScale(animationTime);
        DirectX::XMVECTOR R_quat = InterpolateRotation(animationTime);
        DirectX::XMVECTOR T_vec = InterpolatePosition(animationTime);

        DirectX::XMMATRIX S = DirectX::XMMatrixScalingFromVector(S_vec);
        DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(R_quat);
        DirectX::XMMATRIX T = DirectX::XMMatrixTranslationFromVector(T_vec);
        return S * R * T; // SRT order
    }
};

struct AnimationClip {
    std::string name;
    float durationInTicks;    // Original duration from file
    float ticksPerSecond;     // Animation speed
    std::vector<BoneAnimationChannel> channels;
    // Note: duration in seconds can be calculated as durationInTicks / ticksPerSecond
};

struct BoneInfo {
    int id;                         // Index in the final bone matrices array
    DirectX::XMFLOAT4X4 offsetMatrix; // Inverse Bind Pose Matrix
};

// Represents a node in the skeleton hierarchy (like Assimp's aiNode)
struct SkeletonNode {
    std::string name;
    DirectX::XMFLOAT4X4 transformation; // Bind-pose transformation of this node relative to its parent
    std::vector<SkeletonNode> children;
    // You might add a parent pointer if needed for other operations, but for this Animator,
    // a uni-directional traversal from root to children is sufficient.
};


class Animator {
public:
    Animator(const SkeletonNode* skeletonRoot,
        const std::map<std::string, BoneInfo>* boneInfoMap,
        const DirectX::XMFLOAT4X4& globalInverseTransform); // Typically from your model loader

    void PlayAnimation(const AnimationClip* pAnimation);
    void UpdateAnimation(float dt); // dt is delta time in seconds

    const std::vector<DirectX::XMFLOAT4X4>& GetFinalBoneMatrices() const {
        return m_FinalBoneMatrices;
    }

    bool HasAnimation() const { return m_CurrentAnimation != nullptr; }

private:
    void CalculateBoneTransform(const SkeletonNode* node, const DirectX::XMMATRIX& parentTransform);
    const BoneAnimationChannel* FindBoneChannel(const AnimationClip* animation, const std::string& boneName) const;

    std::vector<DirectX::XMFLOAT4X4> m_FinalBoneMatrices;
    const AnimationClip* m_CurrentAnimation;
    float m_CurrentTimeTicks;
    int m_NumBones; // Cached number of bones from boneInfoMap

    const SkeletonNode* m_SkeletonRootNode; // Root of the skeleton hierarchy
    const std::map<std::string, BoneInfo>* m_BoneInfoMap; // Maps bone name to BoneInfo (ID, offset matrix)
    DirectX::XMMATRIX m_GlobalInverseTransform; // Inverse transform of the model's root node (from Assimp)
};
