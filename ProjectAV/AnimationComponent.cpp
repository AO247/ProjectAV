// AnimationComponent.cpp
#include "AnimationComponent.h"
#include "Node.h"
#include "ModelComponent.h" // To verify skeleton compatibility eventually

// Assimp includes for loading animations
#include <assimp/Importer.hpp>
#include <assimp/scene.h>       // For aiScene, aiAnimation, aiNodeAnim
#include <assimp/postprocess.h> // For aiProcess flags (though fewer are typically needed for anim-only loading)

AnimationComponent::AnimationComponent(Node* owner)
    : Component(owner, "AnimationComponent") 
{
}
bool AnimationComponent::LoadAnimationsFromFile(const std::string& animationFilePath, const std::string& modelFilePathForSkeleton)
{
    if (!mCachedModelComponent) {
        // Initialize() must be called first to link with ModelComponent.
        // However, ModelComponent is mainly for the *target skeleton*.
        // The animation file might be different from the model file,
        // but its animation data must target a compatible skeleton.
        // For now, we'll proceed, but later you might add checks.
        OutputDebugStringA("AnimationComponent Warning: Initialize() not called or ModelComponent missing. Skeleton compatibility cannot be fully verified yet.\n");
    }

    Assimp::Importer importer;
    // Flags for loading animations:
    // aiProcess_PopulateArmatureData can be useful if the animation file ALSO contains a skeleton
    // that needs to be understood by Assimp for node naming, but often animation files
    // might be separate and only contain animation data.
    // aiProcess_ConvertToLeftHanded is important if the node transforms in the animation
    // data are right-handed and need to be used with a left-handed system.
    unsigned int flags = aiProcess_ConvertToLeftHanded; // A good default
    // If you removed aiProcess_PopulateArmatureData from ModelComponent loading due to old Assimp,
    // you might not have it here either. It's less critical for just parsing animation keyframes.

    const aiScene* scene = importer.ReadFile(animationFilePath, flags);

    if (!scene || !scene->HasAnimations()) {
        OutputDebugStringA(("AnimationComponent: No animations found or error loading animation file: " +
            animationFilePath + ". Assimp: " + importer.GetErrorString() + "\n").c_str());
        return false;
    }

    OutputDebugStringA(("AnimationComponent: Found %d animation(s) in file: %s\n", scene->mNumAnimations, animationFilePath.c_str()));
    ProcessAnimations(scene);

    // TODO: Later, you might want to verify that the node names in the loaded animations
    // actually exist in the mCachedModelComponent->m_BoneInfoMap or its node hierarchy.
    // This ensures the animation can actually drive the target skeleton.

    return !mAnimations.empty(); // Return true if at least one animation was loaded
}

void AnimationComponent::ProcessAnimations(const aiScene* scene)
{
    // mAnimations.clear(); // Clear previous animations if loading additively is not desired.
                           // For now, let's assume additive loading, or clear before calling LoadAnimationsFromFile.

    for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
    {
        const aiAnimation* pAIAnim = scene->mAnimations[i];
        Animation newAnim; // Our custom Animation struct

        newAnim.name = pAIAnim->mName.C_Str();
        // If animation has no name, generate one
        if (newAnim.name.empty()) {
            newAnim.name = "Anim_" + std::to_string(mAnimations.size()) + "_" + std::to_string(i);
        }
        OutputDebugStringA(("Processing animation: %s\n", newAnim.name.c_str()));


        newAnim.durationTicks = static_cast<float>(pAIAnim->mDuration);
        newAnim.ticksPerSecond = (pAIAnim->mTicksPerSecond != 0) ?
            static_cast<float>(pAIAnim->mTicksPerSecond) :
            25.0f; // Default to 25 TPS if not specified in file

        // Process all channels (bone/node animations) in this aiAnimation
        for (unsigned int j = 0; j < pAIAnim->mNumChannels; ++j)
        {
            const aiNodeAnim* pAINodeAnim = pAIAnim->mChannels[j]; // Animation data for a single node
            AnimationChannel channel; // Our custom AnimationChannel struct
            channel.nodeName = pAINodeAnim->mNodeName.C_Str();

            // Copy position keyframes
            for (unsigned int k = 0; k < pAINodeAnim->mNumPositionKeys; ++k) {
                channel.positionKeys.emplace_back(
                    static_cast<float>(pAINodeAnim->mPositionKeys[k].mTime),
                    pAINodeAnim->mPositionKeys[k].mValue // Constructor handles aiVector3D -> XMFLOAT3
                );
            }

            // Copy rotation keyframes
            for (unsigned int k = 0; k < pAINodeAnim->mNumRotationKeys; ++k) {
                channel.rotationKeys.emplace_back(
                    static_cast<float>(pAINodeAnim->mRotationKeys[k].mTime),
                    pAINodeAnim->mRotationKeys[k].mValue // Constructor handles aiQuaternion -> XMFLOAT4
                );
            }

            // Copy scaling keyframes
            for (unsigned int k = 0; k < pAINodeAnim->mNumScalingKeys; ++k) {
                channel.scalingKeys.emplace_back(
                    static_cast<float>(pAINodeAnim->mScalingKeys[k].mTime),
                    pAINodeAnim->mScalingKeys[k].mValue // Constructor handles aiVector3D -> XMFLOAT3
                );
            }

            if (!channel.positionKeys.empty() || !channel.rotationKeys.empty() || !channel.scalingKeys.empty()) {
                newAnim.channels.push_back(std::move(channel));
                newAnim.channelMap[channel.nodeName] = newAnim.channels.size() - 1; // Store index for quick lookup
            }
        }
        if (!newAnim.channels.empty()) {
            if (mAnimations.find(newAnim.name) != mAnimations.end()) {
                // Format this debug string too if needed
                std::string warningMsg = "Warning: Duplicate animation name '" + newAnim.name + "' found. Appending unique ID.\n";
                OutputDebugStringA(warningMsg.c_str());
                newAnim.name += "_dup" + std::to_string(mAnimations.size());
            }
            mAnimations[newAnim.name] = std::move(newAnim);
             
            std::ostringstream oss;
            oss << "Successfully processed and stored animation: " << newAnim.name
                << " with " << newAnim.channels.size() << " channels. "
                << "Duration: " << newAnim.durationTicks << " ticks @ "
                << newAnim.ticksPerSecond << " TPS.\n";
            OutputDebugStringA(oss.str().c_str());
 
        }
        else {
            std::string warningMsg = "Warning: Animation '" + newAnim.name + "' had no valid channels.\n";
            OutputDebugStringA(warningMsg.c_str());
        }
    }
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
    // CalculateBoneTransformsRecursive(mCachedModelComponent->GetRootInternalNode(), ¤tAnim, DirectX::XMMatrixIdentity());
    // For now, this call is commented out as CalculateBoneTransformsRecursive isn't fully implemented/debugged.
    // We'll add it in the next phase.

    // If you want to test that mFinalBoneMatrices is being updated, you could temporarily
    // set one of them here, e.g., a slight rotation on bone 0:
    /*
    if (!mFinalBoneMatrices.empty()) {
        DirectX::XMStoreFloat4x4(&mFinalBoneMatrices[0],
            DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(mCurrentTimeTicks * 0.1f)) // Example simple animation
        );
    }
    */
}