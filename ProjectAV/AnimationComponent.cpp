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


// AnimationComponent.cpp

void AnimationComponent::Update(float dt)
{
    if (!mCachedModelComponent) { // Check if ModelComponent is even cached
        // Optionally log that ModelComponent isn't available if it's unexpected
        return;
    }


    if (!mIsPlaying || mCurrentAnimationName.empty() || mCurrentAnimationName == "") {
        // NOT PLAYING: Set mFinalBoneMatrices to represent the bind pose.
        //ModelInternalNode* rootModelNode = mCachedModelComponent->GetRootInternalNode(); // Get it here too
        //if (rootModelNode) {
        //    CalculateBoneTransformsRecursive_BindPose(rootModelNode, DirectX::XMMatrixIdentity());
        //}
        //else {
        //    // Log error if rootModelNode is null even for bind pose
        //    std::ostringstream oss;
        //    oss << "AnimationComponent::Update (Not Playing) - Error: Root node from ModelComponent is null.\n";
        //    OutputDebugStringA(oss.str().c_str());
        //}
        return;
    }

    // --- If Playing ---
    auto animIt = mAnimations.find(mCurrentAnimationName);
    if (animIt == mAnimations.end()) {
        mIsPlaying = false; // Stop if the animation somehow disappeared
        std::ostringstream oss;
        oss << "AnimationComponent::Update - Current animation '" << mCurrentAnimationName << "' not found in map. Stopping playback.\n";
        OutputDebugStringA(oss.str().c_str());
        return;
    }
    const Animation& currentAnim = animIt->second;


    // 1. Advance animation time (your existing logic)
    if (currentAnim.ticksPerSecond > 0.00001f) {
        mCurrentTimeTicks += currentAnim.ticksPerSecond * dt * mAnimationSpeed;
    }

    // 2. Handle looping or end of animation (your existing logic)
    if (mCurrentTimeTicks >= currentAnim.durationTicks) {
        if (mLoop) {
            if (currentAnim.durationTicks > 0.00001f) { // Avoid fmod with zero duration
                mCurrentTimeTicks = fmod(mCurrentTimeTicks, currentAnim.durationTicks);
            }
            else {
                mCurrentTimeTicks = 0.0f; // If duration is zero, just reset
            }
        }
        else {
            mCurrentTimeTicks = currentAnim.durationTicks;
            mIsPlaying = false;
        }
    }

    // 3. ***** CALCULATE FINAL BONE TRANSFORMS for active animation *****
    ModelInternalNode* rootModelNode = mCachedModelComponent->GetRootInternalNode(); // Get the root node
    if (rootModelNode) {
        // Pass the actual root of the ModelComponent's internal hierarchy,
        // the current animation data, and an identity matrix as the initial parent transform.
        CalculateBoneTransformsRecursive(rootModelNode, &currentAnim, DirectX::XMMatrixIdentity());
    }
    else {
        // This shouldn't happen if Initialize() succeeded and ModelComponent is valid and has a root.
        std::ostringstream oss;
        oss << "AnimationComponent::Update (Playing) - Error: Root node from ModelComponent is null.\n";
        OutputDebugStringA(oss.str().c_str());
        mIsPlaying = false; // Stop trying to animate if hierarchy is broken
    }
}
 

// --- Explicit Instantiations (Optional, but good practice if definition is in .cpp) ---
// If you keep the template definition in the .cpp, you might need these for the linker
// to generate code for the specific types you'll use.
//template UINT AnimationComponent::FindKeyframeIndex<PositionKeyframe>(float, const std::vector<PositionKeyframe>&) const;
//template UINT AnimationComponent::FindKeyframeIndex<RotationKeyframe>(float, const std::vector<RotationKeyframe>&) const;
//template UINT AnimationComponent::FindKeyframeIndex<ScalingKeyframe>(float, const std::vector<ScalingKeyframe>&) const;





dx::XMMATRIX AnimationComponent::InterpolatePosition(float animationTimeTicks, const AnimationChannel& channel) const {
    if (channel.positionKeys.empty()) return dx::XMMatrixIdentity();
    if (channel.positionKeys.size() == 1) return dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&channel.positionKeys[0].value));
    UINT p0Index = FindKeyframeIndex(animationTimeTicks, channel.positionKeys);
    UINT p1Index = p0Index + 1;
    if (p1Index >= channel.positionKeys.size()) return dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&channel.positionKeys.back().value));
    const auto& p0 = channel.positionKeys[p0Index];
    const auto& p1 = channel.positionKeys[p1Index];
    float deltaTime = p1.time - p0.time;
    float factor = (deltaTime > 0.00001f) ? std::clamp((animationTimeTicks - p0.time) / deltaTime, 0.0f, 1.0f) : 0.0f;
    return dx::XMMatrixTranslationFromVector(dx::XMVectorLerp(dx::XMLoadFloat3(&p0.value), dx::XMLoadFloat3(&p1.value), factor));
}


dx::XMMATRIX AnimationComponent::InterpolateRotation(float animationTimeTicks, const AnimationChannel& channel) const {
    if (channel.rotationKeys.empty()) return dx::XMMatrixIdentity();
    if (channel.rotationKeys.size() == 1) return dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&channel.rotationKeys[0].value));
    UINT r0Index = FindKeyframeIndex(animationTimeTicks, channel.rotationKeys);
    UINT r1Index = r0Index + 1;
    if (r1Index >= channel.rotationKeys.size()) return dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&channel.rotationKeys.back().value));
    const auto& r0 = channel.rotationKeys[r0Index];
    const auto& r1 = channel.rotationKeys[r1Index];
    float deltaTime = r1.time - r0.time;
    float factor = (deltaTime > 0.00001f) ? std::clamp((animationTimeTicks - r0.time) / deltaTime, 0.0f, 1.0f) : 0.0f;
    // It's good practice to normalize quaternions before slerp if they might not be unit length,
    // though Assimp usually provides normalized ones.
    dx::XMVECTOR q0 = dx::XMQuaternionNormalize(dx::XMLoadFloat4(&r0.value));
    dx::XMVECTOR q1 = dx::XMQuaternionNormalize(dx::XMLoadFloat4(&r1.value));
    return dx::XMMatrixRotationQuaternion(dx::XMQuaternionSlerp(q0, q1, factor));
}

// InterpolateScaling - (ensure this returns XMMatrixScalingFromVector)
dx::XMMATRIX AnimationComponent::InterpolateScaling(float animationTimeTicks, const AnimationChannel& channel) const {
    if (channel.scalingKeys.empty()) return dx::XMMatrixScaling(1.0f, 1.0f, 1.0f); // Default to scale 1
    if (channel.scalingKeys.size() == 1) return dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&channel.scalingKeys[0].value));
    UINT s0Index = FindKeyframeIndex(animationTimeTicks, channel.scalingKeys);
    UINT s1Index = s0Index + 1;
    if (s1Index >= channel.scalingKeys.size()) return dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&channel.scalingKeys.back().value));
    const auto& s0 = channel.scalingKeys[s0Index];
    const auto& s1 = channel.scalingKeys[s1Index];
    float deltaTime = s1.time - s0.time; // ERROR in your previous log, should be s0.time
    // Corrected:
    // float deltaTime = s1.time - s0.time;
    float factor = (deltaTime > 0.00001f) ? std::clamp((animationTimeTicks - s0.time) / deltaTime, 0.0f, 1.0f) : 0.0f;
    return dx::XMMatrixScalingFromVector(dx::XMVectorLerp(dx::XMLoadFloat3(&s0.value), dx::XMLoadFloat3(&s1.value), factor));
}



void AnimationComponent::CalculateBoneTransformsRecursive(
    const ModelInternalNode* modelNode,    // Current node from ModelComponent's hierarchy
    const Animation* pCurrentAnim,         // The active Animation struct
    const DirectX::XMMATRIX& parentGlobalTransform // Global transform of modelNode's parent
) {
    if (!modelNode || !pCurrentAnim || !mCachedModelComponent) {
        return;
    }
    const std::string ARMATURE_ROOT_NODE_NAME = "Armature";
    std::string nodeName = modelNode->GetName();
    DirectX::XMMATRIX nodeLocalAnimatedTransform; // This will be the local S * R * T for this node

    // --- Get Local Animated Transform ---
    auto channelMapEntry = pCurrentAnim->channelMap.find(nodeName);
    if (channelMapEntry != pCurrentAnim->channelMap.end()) {
        // This node IS animated in the current animation
        const AnimationChannel& animChannel = pCurrentAnim->channels[channelMapEntry->second];

        DirectX::XMMATRIX scalingMatrix = InterpolateScaling(mCurrentTimeTicks, animChannel);
        DirectX::XMMATRIX rotationMatrix = InterpolateRotation(mCurrentTimeTicks, animChannel);
        DirectX::XMMATRIX translationMatrix = InterpolatePosition(mCurrentTimeTicks, animChannel);

        if (nodeName == ARMATURE_ROOT_NODE_NAME) {
            scalingMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);
            // You could print here to confirm it's being applied:
            // PrintMatrix(nodeName + " - FORCED Scale Matrix", scalingMatrix);
        }

        nodeLocalAnimatedTransform = scalingMatrix * rotationMatrix * translationMatrix; // Standard S*R*T order
    }
    else {
        // This node is NOT animated in the current animation, so use its bind-pose local transform
        nodeLocalAnimatedTransform = DirectX::XMLoadFloat4x4(&modelNode->GetOriginalTransform_ForAnim());
    }

    // --- Calculate Global Transform for this Node ---
    // Global Transform = Local Animated Transform * Parent's Global Transform
    DirectX::XMMATRIX globalNodeTransform = nodeLocalAnimatedTransform * parentGlobalTransform;

    // --- If this node is a bone, calculate and store its final skinning matrix ---
    const auto& boneInfoMap = mCachedModelComponent->GetBoneInfoMap();
    auto boneInfoIt = boneInfoMap.find(nodeName);

    if (boneInfoIt != boneInfoMap.end()) { // Check if this nodeName is a bone we care about
        const BoneInfo& boneInfo = boneInfoIt->second;
        if (boneInfo.id >= 0 && static_cast<size_t>(boneInfo.id) < mFinalBoneMatrices.size()) {
            // Load the bone's offset matrix (transforms from mesh space to this bone's local space in bind pose)
            // Assimp's mOffsetMatrix is row-major. XMLoadFloat4x4 loads it as such.
            DirectX::XMMATRIX offsetMatrix = DirectX::XMLoadFloat4x4(&boneInfo.offsetMatrix);

            // Final Skinning Matrix for Shader = OffsetMatrix * GlobalAnimatedNodeTransform
            // (offsetMatrix is row-major, globalNodeTransform is row-major, product is row-major)
            DirectX::XMMATRIX finalMatrix = offsetMatrix * globalNodeTransform;

            // Store it transposed for HLSL cbuffer (which expects column-major by default)
            DirectX::XMStoreFloat4x4(
                &mFinalBoneMatrices[boneInfo.id],
                DirectX::XMMatrixTranspose(finalMatrix)
            );

//***** CONDITIONAL DEBUG PRINTING FOR ONE OR TWO BONES *****
            const std::string DEBUG_BONE = "Armature";
            const std::string DEBUG_BONE_1 = "Bone"; // From your charBox3.fbx example
             const std::string DEBUG_BONE_2 = "Bone.001";
             if (nodeName == DEBUG_BONE || nodeName == DEBUG_BONE_1 || nodeName == DEBUG_BONE_2) {
                std::ostringstream oss_debug;
                oss_debug << "\n--- Bone: " << nodeName << " (ID: " << boneInfo.id << ") at t=" << mCurrentTimeTicks << " ---\n";
                OutputDebugStringA(oss_debug.str().c_str());
                PrintMatrix("1. LocalAnimatedSRT", nodeLocalAnimatedTransform);
                PrintMatrix("2. ParentGlobal", parentGlobalTransform);
                PrintMatrix("3. GlobalNodeTransform (Local*ParentGlobal)", globalNodeTransform);
                PrintMatrix("4. OffsetMatrix (from BoneInfo)", offsetMatrix);
                PrintMatrix("5. FinalSkinningMatrix (Offset*Global, Pre-Transpose)", finalMatrix);
                PrintMatrix("6. mFinalBoneMatrices[id] (Post-Transpose, For Shader)", DirectX::XMLoadFloat4x4(&mFinalBoneMatrices[boneInfo.id]));
             }
        }
    }

    // --- Recurse for Children ---
    for (const auto& childNodeUPtr : modelNode->GetChildren_ForAnim()) {
        if (childNodeUPtr) {
            // Pass the current node's globalNodeTransform as the parentGlobalTransform for its children
            CalculateBoneTransformsRecursive(childNodeUPtr.get(), pCurrentAnim, globalNodeTransform);
        }
    }
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
