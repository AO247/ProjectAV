// AnimationComponent.cpp
#include "AnimationComponent.h"
#include "Node.h"
#include "ModelComponent.h" // To verify skeleton compatibility eventually

// Assimp includes for loading animations
#include <assimp/Importer.hpp>
#include <assimp/scene.h>       // For aiScene, aiAnimation, aiNodeAnim
#include <assimp/postprocess.h> // For aiProcess flags (though fewer are typically needed for anim-only loading)
#include <filesystem>
#include <sstream> 

AnimationComponent::AnimationComponent(Node* owner)
    : Component(owner, "AnimationComponent") 
{
}
bool AnimationComponent::LoadAnimationsFromFile(const std::string& animationFilePath, const std::string& modelFilePathForSkeleton)
{
    std::ostringstream oss; // Create a string stream for logging

    if (!mCachedModelComponent) {
        oss << "AnimationComponent Warning: Initialize() not called or ModelComponent missing. Skeleton compatibility cannot be fully verified yet.\n";
        OutputDebugStringA(oss.str().c_str()); oss.str(""); oss.clear(); // Output and clear
    }

    Assimp::Importer importer;
    unsigned int flags = aiProcess_ConvertToLeftHanded;
    const aiScene* scene = importer.ReadFile(animationFilePath, flags);

    if (!scene || !scene->HasAnimations()) {
        oss << "AnimationComponent: No animations found or error loading animation file: "
            << animationFilePath << ". Assimp: " << importer.GetErrorString() << "\n";
        OutputDebugStringA(oss.str().c_str());
        return false;
    }

    oss << "AnimationComponent: Found " << scene->mNumAnimations << " animation(s) in file: " << animationFilePath << "\n";
    OutputDebugStringA(oss.str().c_str()); oss.str(""); oss.clear();

    ProcessAnimations(scene);

    bool success = !mAnimations.empty();
    if (success) {
        oss << "AnimationComponent: Successfully loaded " << mAnimations.size() << " animation(s) into map.\n";
    }
    else {
        oss << "AnimationComponent: ProcessAnimations resulted in an empty animation map.\n";
    }
    OutputDebugStringA(oss.str().c_str());

    return success;
}

void AnimationComponent::ProcessAnimations(const aiScene* scene)
{
    std::ostringstream oss; // Create a string stream for logging within this function

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        const aiAnimation* pAIAnim = scene->mAnimations[i];
        Animation newAnimLocal;

        std::string originalNameFromFile = (pAIAnim->mName.C_Str() && strlen(pAIAnim->mName.C_Str()) > 0) ?
            pAIAnim->mName.C_Str() :
            "UnnamedAssimpAnim_" + std::to_string(i);

        oss << "--- Processing Assimp Animation: '" << originalNameFromFile
            << "', NumChannels in aiAnimation: " << pAIAnim->mNumChannels << "\n";
        OutputDebugStringA(oss.str().c_str()); oss.str(""); oss.clear();


        newAnimLocal.name = originalNameFromFile;
        // If it was unnamed by Assimp, ensure our internal name is unique enough
        if (originalNameFromFile.rfind("UnnamedAssimpAnim_", 0) == 0 && mAnimations.count(newAnimLocal.name)) {
            newAnimLocal.name += "_inst" + std::to_string(mAnimations.size());
        }


        newAnimLocal.durationTicks = static_cast<float>(pAIAnim->mDuration);
        newAnimLocal.ticksPerSecond = (pAIAnim->mTicksPerSecond != 0) ?
            static_cast<float>(pAIAnim->mTicksPerSecond) :
            25.0f;

        for (unsigned int j = 0; j < pAIAnim->mNumChannels; ++j)
        {
            const aiNodeAnim* pAINodeAnim = pAIAnim->mChannels[j];
            AnimationChannel channel;
            channel.nodeName = pAINodeAnim->mNodeName.C_Str();

            for (unsigned int k = 0; k < pAINodeAnim->mNumPositionKeys; ++k) {
                channel.positionKeys.emplace_back(
                    static_cast<float>(pAINodeAnim->mPositionKeys[k].mTime),
                    pAINodeAnim->mPositionKeys[k].mValue
                );
            }
            for (unsigned int k = 0; k < pAINodeAnim->mNumRotationKeys; ++k) {
                channel.rotationKeys.emplace_back(
                    static_cast<float>(pAINodeAnim->mRotationKeys[k].mTime),
                    pAINodeAnim->mRotationKeys[k].mValue
                );
            }
            for (unsigned int k = 0; k < pAINodeAnim->mNumScalingKeys; ++k) {
                channel.scalingKeys.emplace_back(
                    static_cast<float>(pAINodeAnim->mScalingKeys[k].mTime),
                    pAINodeAnim->mScalingKeys[k].mValue
                );
            }

            if (!channel.positionKeys.empty() || !channel.rotationKeys.empty() || !channel.scalingKeys.empty()) {
                newAnimLocal.channels.push_back(std::move(channel));
                // We'll build the channelMap after all channels for newAnimLocal are collected,
                // to ensure indices are correct if some pAINodeAnim channels were empty.
            }
        } // End loop through pAINodeAnim channels

        // Rebuild channelMap for newAnimLocal
        newAnimLocal.channelMap.clear();
        for (size_t chIdx = 0; chIdx < newAnimLocal.channels.size(); ++chIdx) {
            newAnimLocal.channelMap[newAnimLocal.channels[chIdx].nodeName] = chIdx;
        }


        if (!newAnimLocal.channels.empty()) {
            std::string finalAnimationName = newAnimLocal.name;

            if (mAnimations.count(finalAnimationName)) {
                std::string baseName = newAnimLocal.name;
                int attempt = 1;
                oss.str(""); oss.clear(); // Clear oss before using for this specific log
                oss << "Warning: Duplicate animation name '" << newAnimLocal.name << "' found. ";
                do {
                    finalAnimationName = baseName + "_dup" + std::to_string(attempt++);
                } while (mAnimations.count(finalAnimationName));
                oss << "Renaming to '" << finalAnimationName << "'.\n";
                OutputDebugStringA(oss.str().c_str());
                newAnimLocal.name = finalAnimationName;
            }

            size_t numChannelsForLog = newAnimLocal.channels.size();
            float durationForLog = newAnimLocal.durationTicks;
            float tpsForLog = newAnimLocal.ticksPerSecond;

            mAnimations[finalAnimationName] = std::move(newAnimLocal);

            oss.str(""); oss.clear();
            oss << "Successfully processed and stored animation: " << finalAnimationName
                << " with " << numChannelsForLog << " channels. Duration: " << durationForLog
                << " ticks @ " << tpsForLog << " TPS.\n";
            OutputDebugStringA(oss.str().c_str());
        }
        else {
            oss.str(""); oss.clear();
            oss << "Warning: Animation '" << newAnimLocal.name
                << "' (from file original: '" << originalNameFromFile
                << "') had no valid channels and was not stored.\n";
            OutputDebugStringA(oss.str().c_str());
        }
    } // End loop through pAIAnim
}

void AnimationComponent::Initialize()
{
    if (!pOwner) // pOwner is the Node this component is attached to (from base Component class)
    {
        OutputDebugStringA("AnimationComponent Error: Owner Node is null during Initialize().\n");
        return;
    }

    mCachedModelComponent = pOwner->GetComponent<ModelComponent>(); // Attempt to get the sibling ModelComponent

    if (!mCachedModelComponent)
    {
        OutputDebugStringA("AnimationComponent Error: ModelComponent not found on the same Node during Initialize(). Animations may not work.\n");
        // Depending on your design, you might want to disable the AnimationComponent
        // or throw an exception if a ModelComponent is strictly required.
        return;
    }

    // Now that we have the ModelComponent, we know the actual number of bones.
    // Resize and initialize mFinalBoneMatrices.
    const int boneCount = mCachedModelComponent->GetBoneCount();

    if (boneCount > 0)
    {
        // Resize to the actual number of bones, but cap it at what the shader/component can handle.
        mFinalBoneMatrices.resize(std::min(boneCount, MAX_ANIM_COMPONENT_BONES));
        std::ostringstream oss;
        oss << "AnimationComponent: Resized mFinalBoneMatrices for " << boneCount
            << " bones (capped at " << MAX_ANIM_COMPONENT_BONES << ").\n";
        OutputDebugStringA(oss.str().c_str());
    }
    else
    {
        mFinalBoneMatrices.clear(); // No bones, no matrices needed.
        OutputDebugStringA("AnimationComponent: ModelComponent reported 0 bones. mFinalBoneMatrices cleared.\n");
    }

    // Initialize all matrices to identity.
    for (auto& matrix : mFinalBoneMatrices)
    {
        DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
    }

    OutputDebugStringA("AnimationComponent Initialized.\n");
}


void AnimationComponent::Update(float dt)
{
    // This is where the animation playback logic will go.
    // For now, it might be empty or just handle time progression.

    if (!mIsPlaying || mCurrentAnimationName.empty() || !mCachedModelComponent)
    {
        // If not playing, or no current animation, or no model component,
        // then there's nothing to update regarding animation transforms.
        // You might want to ensure bones are in a T-pose/bind-pose here if desired when not playing.
        return;
    }

    // Get the current animation
    auto animIt = mAnimations.find(mCurrentAnimationName);
    if (animIt == mAnimations.end())
    {
        mIsPlaying = false; // Animation disappeared or was never found
        OutputDebugStringA(("AnimationComponent::Update - Current animation '%s' not found in map.\n", mCurrentAnimationName.c_str()));
        return;
    }
    const Animation& currentAnim = animIt->second;

    // 1. Advance animation time
    if (currentAnim.ticksPerSecond > 0.00001f) // Avoid division by zero if TPS is 0
    {
        mCurrentTimeTicks += currentAnim.ticksPerSecond * dt * mAnimationSpeed;
    }
    else // If ticksPerSecond is 0, animation time doesn't advance based on dt
    {
        // Potentially advance mCurrentTimeTicks by a fixed amount or handle as error
    }

    if (mIsPlaying && !mCurrentAnimationName.empty()) // Only log if actually attempting to play
    {
        // TEMPORARY LOGGING - REMOVE LATER
        std::ostringstream oss;
        if (!mFinalBoneMatrices.empty()) {
            oss << "  Matrices updated this frame. Count: " << mFinalBoneMatrices.size() << "\n";
            // Log first few bones and maybe a middle one if many bones
            for (size_t i = 0; i < std::min((size_t)5, mFinalBoneMatrices.size()); ++i) { // Log up to first 5
                const auto& mat_s = mFinalBoneMatrices[i]; // mat_s for 'stored matrix'
                // Quick check if it's roughly identity (for quick visual scan in logs)
                bool isIdentityIsh = (fabs(mat_s._11 - 1.0f) < 0.01f && fabs(mat_s._22 - 1.0f) < 0.01f &&
                    fabs(mat_s._33 - 1.0f) < 0.01f && fabs(mat_s._44 - 1.0f) < 0.01f &&
                    fabs(mat_s._41) < 0.01f && fabs(mat_s._42) < 0.01f && fabs(mat_s._43) < 0.01f);

                oss << "    Bone[" << i << "] (row1): "
                    << mat_s._11 << " " << mat_s._12 << " " << mat_s._13 << " " << mat_s._14
                    << (isIdentityIsh ? " (Looks Identity-ish)" : " (Looks Animated)") << "\n";
            }
            if (mFinalBoneMatrices.size() > 10) { // Example: log a middle bone if there are many
                size_t mid_idx = mFinalBoneMatrices.size() / 2;
                const auto& mat_s_mid = mFinalBoneMatrices[mid_idx];
                bool isIdentityIshMid = (fabs(mat_s_mid._11 - 1.0f) < 0.01f && fabs(mat_s_mid._22 - 1.0f) < 0.01f &&
                    fabs(mat_s_mid._33 - 1.0f) < 0.01f && fabs(mat_s_mid._44 - 1.0f) < 0.01f &&
                    fabs(mat_s_mid._41) < 0.01f && fabs(mat_s_mid._42) < 0.01f && fabs(mat_s_mid._43) < 0.01f);
                oss << "    Bone[" << mid_idx << "] (row1): "
                    << mat_s_mid._11 << " " << mat_s_mid._12 << " " << mat_s_mid._13 << " " << mat_s_mid._14
                    << (isIdentityIshMid ? " (Looks Identity-ish)" : " (Looks Animated)") << "\n";
            }
        }
        OutputDebugStringA(oss.str().c_str());
    }

    ModelInternalNode* rootModelNode = mCachedModelComponent->GetRootInternalNode();
    if (mIsPlaying && rootModelNode && animIt != mAnimations.end()) { // Ensure animIt is valid
        const Animation& currentAnim = animIt->second;
        CalculateBoneTransformsRecursive(rootModelNode, &currentAnim, DirectX::XMMatrixIdentity());
    }

    // 2. Handle looping or end of animation
    if (mCurrentTimeTicks >= currentAnim.durationTicks)
    {
        if (mLoop)
        {
            // Loop back: fmod works, or simply subtract duration until within range
            mCurrentTimeTicks = fmod(mCurrentTimeTicks, currentAnim.durationTicks);
            // Or: while (mCurrentTimeTicks >= currentAnim.durationTicks) mCurrentTimeTicks -= currentAnim.durationTicks;
            // (Ensure durationTicks is not zero to avoid infinite loop if mCurrentTimeTicks is also zero)
        }
        else
        {
            mCurrentTimeTicks = currentAnim.durationTicks; // Clamp to the end
            mIsPlaying = false;                            // Stop playing
            // Optionally, fire an "animation ended" event here
        }
    }

    // 3. Calculate final bone transforms (This will be a call to a more complex function)
   // ModelInternalNode* rootModelNode = mCachedModelComponent->GetRootInternalNode();
    if (rootModelNode) {
        CalculateBoneTransformsRecursive(rootModelNode, &currentAnim, dx::XMMatrixIdentity());


    }
    else {
        // This shouldn't happen if Initialize() succeeded and ModelComponent is valid
        OutputDebugStringA("AnimationComponent::Update - Error: Root node from ModelComponent is null.\n");
        mIsPlaying = false; // Stop trying to animate
    }

}
 

// --- Explicit Instantiations (Optional, but good practice if definition is in .cpp) ---
// If you keep the template definition in the .cpp, you might need these for the linker
// to generate code for the specific types you'll use.
template UINT AnimationComponent::FindKeyframeIndex<PositionKeyframe>(float, const std::vector<PositionKeyframe>&) const;
template UINT AnimationComponent::FindKeyframeIndex<RotationKeyframe>(float, const std::vector<RotationKeyframe>&) const;
template UINT AnimationComponent::FindKeyframeIndex<ScalingKeyframe>(float, const std::vector<ScalingKeyframe>&) const;

dx::XMMATRIX AnimationComponent::InterpolatePosition(float animationTimeTicks, const AnimationChannel& channel) const
{
    if (channel.positionKeys.empty()) {
        return dx::XMMatrixIdentity(); // No position data, return identity
    }
    if (channel.positionKeys.size() == 1) {
        // Only one key, use its value directly
        return dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&channel.positionKeys[0].value));
    }

    UINT p0Index = FindKeyframeIndex(animationTimeTicks, channel.positionKeys);
    UINT p1Index = p0Index + 1;

    // If animationTimeTicks is at or after the last keyframe, use the last keyframe's value
    if (p1Index >= channel.positionKeys.size()) {
        return dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&channel.positionKeys.back().value));
    }

    const PositionKeyframe& p0 = channel.positionKeys[p0Index];
    const PositionKeyframe& p1 = channel.positionKeys[p1Index];

    float deltaTime = p1.time - p0.time;
    float factor = 0.0f;
    if (deltaTime > 0.00001f) { // Avoid division by zero
        factor = (animationTimeTicks - p0.time) / deltaTime;
    }
    factor = std::clamp(factor, 0.0f, 1.0f); // Ensure factor is [0, 1]

    dx::XMVECTOR pos0 = dx::XMLoadFloat3(&p0.value);
    dx::XMVECTOR pos1 = dx::XMLoadFloat3(&p1.value);
    dx::XMVECTOR interpolatedPosition = dx::XMVectorLerp(pos0, pos1, factor);

    return dx::XMMatrixTranslationFromVector(interpolatedPosition);
}

dx::XMMATRIX AnimationComponent::InterpolateRotation(float animationTimeTicks, const AnimationChannel& channel) const
{
    if (channel.rotationKeys.empty()) {
        return dx::XMMatrixIdentity(); // No rotation data
    }
    if (channel.rotationKeys.size() == 1) {
        return dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&channel.rotationKeys[0].value));
    }

    UINT r0Index = FindKeyframeIndex(animationTimeTicks, channel.rotationKeys);
    UINT r1Index = r0Index + 1;

    if (r1Index >= channel.rotationKeys.size()) {
        return dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&channel.rotationKeys.back().value));
    }

    const RotationKeyframe& r0 = channel.rotationKeys[r0Index];
    const RotationKeyframe& r1 = channel.rotationKeys[r1Index];

    float deltaTime = r1.time - r0.time;
    float factor = 0.0f;
    if (deltaTime > 0.00001f) {
        factor = (animationTimeTicks - r0.time) / deltaTime;
    }
    factor = std::clamp(factor, 0.0f, 1.0f);

    dx::XMVECTOR rot0 = dx::XMLoadFloat4(&r0.value);
    dx::XMVECTOR rot1 = dx::XMLoadFloat4(&r1.value);
    // Ensure quaternions are normalized before slerp for stability, though Assimp usually provides normalized ones.
    // rot0 = dx::XMQuaternionNormalize(rot0);
    // rot1 = dx::XMQuaternionNormalize(rot1);
    dx::XMVECTOR interpolatedRotation = dx::XMQuaternionSlerp(rot0, rot1, factor);

    return dx::XMMatrixRotationQuaternion(interpolatedRotation);
}

dx::XMMATRIX AnimationComponent::InterpolateScaling(float animationTimeTicks, const AnimationChannel& channel) const
{
    if (channel.scalingKeys.empty()) {
       // return dx::XMMatrixIdentity(); // Or XMMatrixScaling(1.0f, 1.0f, 1.0f)
        return dx::XMMatrixScaling(1.0f, 1.0f, 1.0f);
    }
    if (channel.scalingKeys.size() == 1) {
        return dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&channel.scalingKeys[0].value));
    }

    UINT s0Index = FindKeyframeIndex(animationTimeTicks, channel.scalingKeys);
    UINT s1Index = s0Index + 1;

    if (s1Index >= channel.scalingKeys.size()) {
        return dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&channel.scalingKeys.back().value));
    }

    const ScalingKeyframe& s0 = channel.scalingKeys[s0Index];
    const ScalingKeyframe& s1 = channel.scalingKeys[s1Index];

    float deltaTime = s1.time - s0.time;
    float factor = 0.0f;
    if (deltaTime > 0.00001f) {
        factor = (animationTimeTicks - s0.time) / deltaTime;
    }
    factor = std::clamp(factor, 0.0f, 1.0f);

    dx::XMVECTOR scale0 = dx::XMLoadFloat3(&s0.value);
    dx::XMVECTOR scale1 = dx::XMLoadFloat3(&s1.value);
    dx::XMVECTOR interpolatedScaling = dx::XMVectorLerp(scale0, scale1, factor);

    return dx::XMMatrixScalingFromVector(interpolatedScaling);
}


void AnimationComponent::CalculateBoneTransformsRecursive(
    const ModelInternalNode* modelNode,
    const Animation* pCurrentAnim,
    const dx::XMMATRIX& parentGlobalTransform
) {
    if (!modelNode || !pCurrentAnim || !mCachedModelComponent) { // Basic safety checks
        return;
    }

    std::string nodeName = modelNode->GetName(); // Assuming ModelInternalNode has GetName()

    // Start with the node's original bind-pose transform from the model file
    dx::XMMATRIX nodeLocalAnimatedTransform = modelNode->GetOriginalTransformXM();

    // Check if this node has an animation channel in the current animation
    auto channelMapEntry = pCurrentAnim->channelMap.find(nodeName);
    if (channelMapEntry != pCurrentAnim->channelMap.end()) {
        const AnimationChannel& animChannel = pCurrentAnim->channels[channelMapEntry->second];

        // Get interpolated S, R, T matrices
        // Print original keyframe values before interpolation
        if (!animChannel.scalingKeys.empty()) {
            const auto& firstScale = animChannel.scalingKeys.front().value;
            const auto& lastScale = animChannel.scalingKeys.back().value;
            std::ostringstream oss;
            oss << "Node: " << nodeName << " ScalingKeyframes: "
                << "First: [" << firstScale.x << ", " << firstScale.y << ", " << firstScale.z << "] "
                << "Last: [" << lastScale.x << ", " << lastScale.y << ", " << lastScale.z << "]\n";
            OutputDebugStringA(oss.str().c_str());
        }
        if (!animChannel.rotationKeys.empty()) {
            const auto& firstRot = animChannel.rotationKeys.front().value;
            const auto& lastRot = animChannel.rotationKeys.back().value;
            std::ostringstream oss;
            oss << "Node: " << nodeName << " RotationKeyframes: "
                << "First: [" << firstRot.x << ", " << firstRot.y << ", " << firstRot.z << ", " << firstRot.w << "] "
                << "Last: [" << lastRot.x << ", " << lastRot.y << ", " << lastRot.z << ", " << lastRot.w << "]\n";
            OutputDebugStringA(oss.str().c_str());
        }
        if (!animChannel.positionKeys.empty()) {
            const auto& firstPos = animChannel.positionKeys.front().value;
            const auto& lastPos = animChannel.positionKeys.back().value;
            std::ostringstream oss;
            oss << "Node: " << nodeName << " PositionKeyframes: "
                << "First: [" << firstPos.x << ", " << firstPos.y << ", " << firstPos.z << "] "
                << "Last: [" << lastPos.x << ", " << lastPos.y << ", " << lastPos.z << "]\n";
            OutputDebugStringA(oss.str().c_str());
        }

        dx::XMMATRIX scalingMatrix = InterpolateScaling(mCurrentTimeTicks, animChannel);
        dx::XMMATRIX rotationMatrix = InterpolateRotation(mCurrentTimeTicks, animChannel);
        dx::XMMATRIX translationMatrix = InterpolatePosition(mCurrentTimeTicks, animChannel);

        //if (nodeName == "Armature") { // Or your armature root name
        //    scalingMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f); // Force scale to 1
        //}
        // Debug: Print the S, R, T matrices for this node/channel
        {
            std::ostringstream oss;
            oss << "Node: " << nodeName << " Animation Matrices at t=" << mCurrentTimeTicks << "\n";
            DirectX::XMFLOAT4X4 matData;

            DirectX::XMStoreFloat4x4(&matData, scalingMatrix);
            oss << "  ScalingMatrix:\n";
            oss << "    [" << matData._11 << ", " << matData._12 << ", " << matData._13 << ", " << matData._14 << "]\n";
            oss << "    [" << matData._21 << ", " << matData._22 << ", " << matData._23 << ", " << matData._24 << "]\n";
            oss << "    [" << matData._31 << ", " << matData._32 << ", " << matData._33 << ", " << matData._34 << "]\n";
            oss << "    [" << matData._41 << ", " << matData._42 << ", " << matData._43 << ", " << matData._44 << "]\n";

            DirectX::XMStoreFloat4x4(&matData, rotationMatrix);
            oss << "  RotationMatrix:\n";
            oss << "    [" << matData._11 << ", " << matData._12 << ", " << matData._13 << ", " << matData._14 << "]\n";
            oss << "    [" << matData._21 << ", " << matData._22 << ", " << matData._23 << ", " << matData._24 << "]\n";
            oss << "    [" << matData._31 << ", " << matData._32 << ", " << matData._33 << ", " << matData._34 << "]\n";
            oss << "    [" << matData._41 << ", " << matData._42 << ", " << matData._43 << ", " << matData._44 << "]\n";

            DirectX::XMStoreFloat4x4(&matData, translationMatrix);
            oss << "  TranslationMatrix:\n";
            oss << "    [" << matData._11 << ", " << matData._12 << ", " << matData._13 << ", " << matData._14 << "]\n";
            oss << "    [" << matData._21 << ", " << matData._22 << ", " << matData._23 << ", " << matData._24 << "]\n";
            oss << "    [" << matData._31 << ", " << matData._32 << ", " << matData._33 << ", " << matData._34 << "]\n";
            oss << "    [" << matData._41 << ", " << matData._42 << ", " << matData._43 << ", " << matData._44 << "]\n";

            OutputDebugStringA(oss.str().c_str());
        }

        // Combine: Scale -> Rotate -> Translate for the local animated transform
        nodeLocalAnimatedTransform = scalingMatrix *   rotationMatrix * translationMatrix;
        //nodeLocalAnimatedTransform = translationMatrix;
    }
    // If no animation channel for this node, nodeLocalAnimatedTransform remains its bind-pose transform.

    // Global transform for this node: local animated transform * parent's global transform
    dx::XMMATRIX globalNodeTransform = nodeLocalAnimatedTransform * parentGlobalTransform;

    // If this node corresponds to a bone in our m_BoneInfoMap, calculate its final skinning matrix
    const auto& boneInfoMap = mCachedModelComponent->GetBoneInfoMap(); // Get from cached ModelComponent
    auto boneInfoIt = boneInfoMap.find(nodeName);

    if (boneInfoIt != boneInfoMap.end()) {
        const BoneInfo& boneInfo = boneInfoIt->second;
        if (boneInfo.id >= 0 && static_cast<size_t>(boneInfo.id) < mFinalBoneMatrices.size()) {
            // Final skinning matrix = OffsetMatrix * GlobalAnimatedNodeTransform
            // Assimp's offsetMatrix is row-major. GlobalNodeTransform is likely row-major from DirectXMath.
            // Shader usually expects column-major.
            dx::XMMATRIX offsetMat = dx::XMLoadFloat4x4(&boneInfo.offsetMatrix); // Load as is (row-major)

            // The result of (offsetMat * globalNodeTransform) transforms from bind-pose mesh space to final animated pose.
            // Then, this matrix is applied to vertices in the shader.
            // Transpose the final result for HLSL constant buffers (which expect column-major by default).
            dx::XMStoreFloat4x4(
                &mFinalBoneMatrices[boneInfo.id],
                dx::XMMatrixTranspose(offsetMat * globalNodeTransform)
            );
        }
    }

    // Recursively call for all children of this modelNode
    // Assuming ModelInternalNode has a way to access its children, e.g., a GetChildren() method
    // or if childPtrs is public (less ideal) or AnimationComponent is a friend.
    // For now, using the structure from your ModelComponent.h where childPtrs was:
    // std::vector<std::unique_ptr<ModelInternalNode>> childPtrs;
    // And ModelInternalNode has a friend class ModelComponent.
    // To access children from AnimationComponent, ModelInternalNode needs a getter or friend status.
    // Let's assume a getter: const auto& children = modelNode->GetChildren();
    for (const auto& childUPtr : modelNode->GetChildren()) { // Replace GetChildren_ForAnim with actual getter
        if (childUPtr) {
            CalculateBoneTransformsRecursive(childUPtr.get(), pCurrentAnim, globalNodeTransform);
        }
    }

}

void PrintMatrix(const std::string& matrixName, const DirectX::XMMATRIX& matrix) {
    std::ostringstream oss;
    oss << "Matrix: " << matrixName << std::fixed << std::setprecision(3) << "\n";
    DirectX::XMFLOAT4X4 matData;
    DirectX::XMStoreFloat4x4(&matData, matrix);
    oss << "[" << matData._11 << ", " << matData._12 << ", " << matData._13 << ", " << matData._14 << "]\n";
    oss << "[" << matData._21 << ", " << matData._22 << ", " << matData._23 << ", " << matData._24 << "]\n";
    oss << "[" << matData._31 << ", " << matData._32 << ", " << matData._33 << ", " << matData._34 << "]\n";
    oss << "[" << matData._41 << ", " << matData._42 << ", " << matData._43 << ", " << matData._44 << "]\n\n";
    OutputDebugStringA(oss.str().c_str());
}

void AnimationComponent::PlayAnimation(const std::string& animationName, bool loop /*= true*/)
{
    auto animIt = mAnimations.find(animationName);
    if (animIt != mAnimations.end()) {
        mCurrentAnimationName = animationName;
        mCurrentTimeTicks = 0.0f; // Reset time to start of animation
        mIsPlaying = true;
        mLoop = loop;
        std::ostringstream oss;
        oss << "AnimationComponent: Playing animation '" << animationName << "'. Loop: " << (loop ? "Yes" : "No") << "\n";
        OutputDebugStringA(oss.str().c_str());
    }
    else {
        std::ostringstream oss;
        oss << "AnimationComponent Warning: Attempted to play unknown animation '" << animationName << "'.\n";
        OutputDebugStringA(oss.str().c_str());
        mIsPlaying = false; // Ensure it's not playing if anim not found
    }
}
