#include "Animator.h" // Or whatever you named your header file
#include <iostream> // For potential debug output

Animator::Animator(const SkeletonNode* skeletonRoot,
    const std::map<std::string, BoneInfo>* boneInfoMap,
    const DirectX::XMFLOAT4X4& globalInverseTransform)
    : m_CurrentAnimation(nullptr),
    m_CurrentTimeTicks(0.0f),
    m_SkeletonRootNode(skeletonRoot),
    m_BoneInfoMap(boneInfoMap),
    m_NumBones(0)
{
    if (m_BoneInfoMap) {
        m_NumBones = static_cast<int>(m_BoneInfoMap->size());
    }
    else {
        // Handle error: boneInfoMap is null
        std::cerr << "Animator Error: BoneInfoMap is null." << std::endl;
    }

    // It's important that m_NumBones is <= MAX_BONES_PER_MESH
    // If you have more bones than your shader supports, you'll need a strategy
    // (e.g., error, or only animate the first MAX_BONES_PER_MESH)
    if (m_NumBones > MAX_BONES_PER_MESH) {
        std::cerr << "Animator Warning: Number of bones (" << m_NumBones
            << ") exceeds MAX_BONES_PER_MESH (" << MAX_BONES_PER_MESH
            << "). Truncating or errors may occur." << std::endl;
        // Consider how to handle this. For now, we'll still size to m_NumBones,
        // but the shader will only use up to MAX_BONES_PER_MESH.
        // A better approach might be to cap m_NumBones here or ensure loader respects this.
    }


    m_FinalBoneMatrices.resize(std::max(1, m_NumBones), DirectX::XMFLOAT4X4()); // Ensure at least one, even if no bones
    // Initialize with identity if you want, though they get overwritten each frame
    DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
    for (int i = 0; i < m_FinalBoneMatrices.size(); ++i) {
        DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[i], identity);
    }

    m_GlobalInverseTransform = DirectX::XMLoadFloat4x4(&globalInverseTransform);
}

void Animator::PlayAnimation(const AnimationClip* pAnimation) {
    m_CurrentAnimation = pAnimation;
    m_CurrentTimeTicks = 0.0f; // Reset time when a new animation starts

    // Initialize to bind pose if no animation or at time 0
    if (!m_CurrentAnimation && m_SkeletonRootNode) {
        CalculateBoneTransform(m_SkeletonRootNode, DirectX::XMMatrixIdentity());
    }
}

void Animator::UpdateAnimation(float dt) {
    if (!m_CurrentAnimation || !m_SkeletonRootNode || !m_BoneInfoMap || m_BoneInfoMap->empty()) {
        // If no animation or skeleton, ensure final matrices are identity or bind pose
        // (Often, you might render a static pose if no animation is playing)
        // For now, just return if nothing to animate.
        // If you always want a T-pose or bind-pose, you could call CalculateBoneTransform
        // with bind-pose transforms (node->transformation) and skip interpolation.
        return;
    }

    float ticksPerSecond = m_CurrentAnimation->ticksPerSecond != 0 ? m_CurrentAnimation->ticksPerSecond : 25.0f; // Default to 25 TPS if not specified
    m_CurrentTimeTicks += dt * ticksPerSecond;
    m_CurrentTimeTicks = fmod(m_CurrentTimeTicks, m_CurrentAnimation->durationInTicks); // Loop animation

    CalculateBoneTransform(m_SkeletonRootNode, DirectX::XMMatrixIdentity());
}

void Animator::CalculateBoneTransform(const SkeletonNode* node, const DirectX::XMMATRIX& parentTransform) {
    if (!node) return;

    std::string nodeName = node->name;
    DirectX::XMMATRIX nodeTransform = DirectX::XMLoadFloat4x4(&node->transformation); // Default bind-pose transform

    const BoneAnimationChannel* pNodeAnim = FindBoneChannel(m_CurrentAnimation, nodeName);
    if (pNodeAnim) {
        // Get interpolated local transform for this bone at the current animation time
        nodeTransform = pNodeAnim->GetInterpolatedTransform(m_CurrentTimeTicks);
    }

    // Global transform for this node: parent's global transform * this node's local transform
    DirectX::XMMATRIX globalTransformation = DirectX::XMMatrixMultiply(nodeTransform, parentTransform);

    // If this node is a bone that has skinning data
    auto boneInfoIt = m_BoneInfoMap->find(nodeName);
    if (boneInfoIt != m_BoneInfoMap->end()) {
        const BoneInfo& boneInfo = boneInfoIt->second;
        if (boneInfo.id < m_FinalBoneMatrices.size()) { // Check bounds
            DirectX::XMMATRIX offsetMatrix = DirectX::XMLoadFloat4x4(&boneInfo.offsetMatrix);

            // Final transformation for skinning:
            // ModelViewSpaceBoneTransform = GlobalInverseRootTransform * GlobalBoneTransform * InverseBindPoseMatrix
            // The GlobalInverseRootTransform is to bring everything into the space of the animated model itself,
            // relative to how Assimp presented its root node.
            // The globalTransformation is already in this space if parentTransform started as identity from the root.
            DirectX::XMMATRIX finalMatrix = DirectX::XMMatrixMultiply(offsetMatrix, globalTransformation);
            finalMatrix = DirectX::XMMatrixMultiply(finalMatrix, m_GlobalInverseTransform); // Apply global inverse at the end for correct space

            // HLSL expects column-major matrices by default. DirectXMath uses row-major.
            // So, transpose before sending to the shader.
            DirectX::XMStoreFloat4x4(&m_FinalBoneMatrices[boneInfo.id], DirectX::XMMatrixTranspose(finalMatrix));
        }
        else {
            // This should not happen if m_BoneInfoMap IDs are correctly 0-indexed and m_FinalBoneMatrices is sized correctly.
            std::cerr << "Animator Error: Bone ID " << boneInfo.id << " out of bounds for m_FinalBoneMatrices (size " << m_FinalBoneMatrices.size() << ")." << std::endl;
        }
    }
    else {
        // This node might be part of the hierarchy but not directly a bone influencing the mesh
        // (e.g., an empty used as a controller, or a node between bones).
        // Its transform is still important for its children.
    }

    // Recursively process all children
    for (const auto& child : node->children) {
        CalculateBoneTransform(&child, globalTransformation);
    }
}

const BoneAnimationChannel* Animator::FindBoneChannel(const AnimationClip* animation, const std::string& boneName) const {
    if (!animation) return nullptr;

    for (const auto& channel : animation->channels) {
        if (channel.boneName == boneName) {
            return &channel;
        }
    }
    return nullptr; // Bone not animated in this clip (will use its bind pose relative transform)
}