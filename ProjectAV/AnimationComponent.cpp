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
    unsigned int flags = aiProcess_ConvertToLeftHanded | aiProcess_Triangulate;
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
    for (auto& matrix : mFinalBoneMatrices) {
        DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
    }

    OutputDebugStringA("AnimationComponent Initialized.\n");
}


// AnimationComponent.cpp

void AnimationComponent::Update(float dt) {
    // ... (Your existing logic that sets pAnimToProcess to nullptr for bind pose
    //      and calls CalculateBoneTransformsRecursive) ...
    // This was Recommendation 2 and seems correct from your provided code.
    if (!mCachedModelComponent) {
        return;
    }

    const Animation* pAnimToProcess = nullptr;
    float timeTicksForCalc = 0.0f;

    if (mIsPlaying && !mCurrentAnimationName.empty()) {
        auto animIt = mAnimations.find(mCurrentAnimationName);
        if (animIt != mAnimations.end()) {
            const Animation& currentAnim = animIt->second;
            if (currentAnim.ticksPerSecond > 0.00001f) {
                mCurrentTimeTicks += currentAnim.ticksPerSecond * dt * mAnimationSpeed;
            }
            if (mCurrentTimeTicks >= currentAnim.durationTicks) {
                if (mLoop) {
                    mCurrentTimeTicks = (currentAnim.durationTicks > 0.00001f) ?
                        fmod(mCurrentTimeTicks, currentAnim.durationTicks) : 0.0f;
                }
                else {
                    mCurrentTimeTicks = currentAnim.durationTicks;
                    mIsPlaying = false;
                }
            }
            if (mIsPlaying) {
                pAnimToProcess = &currentAnim;
                timeTicksForCalc = mCurrentTimeTicks;
            }
        }
        else {
            std::ostringstream oss;
            oss << "AnimationComponent::Update - Current animation '" << mCurrentAnimationName << "' not found. Reverting to bind pose.\n";
            OutputDebugStringA(oss.str().c_str());
            mIsPlaying = false;
            mCurrentAnimationName = "";
        }
    }

    ModelInternalNode* rootModelNode = mCachedModelComponent->GetRootInternalNode();
    if (rootModelNode) {
        float previousComponentTime = mCurrentTimeTicks;
        mCurrentTimeTicks = timeTicksForCalc;
        CalculateBoneTransformsRecursive(rootModelNode, pAnimToProcess, DirectX::XMMatrixIdentity());
        mCurrentTimeTicks = previousComponentTime;
    }
    else {
        if (mIsPlaying || pAnimToProcess) {
            std::ostringstream oss;
            oss << "AnimationComponent::Update - Error: Root node from ModelComponent is null.\n";
            OutputDebugStringA(oss.str().c_str());
            mIsPlaying = false;
        }
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
    const ModelInternalNode* modelNode,
    const Animation* pCurrentAnim, // Will be nullptr for bind pose
    const DirectX::XMMATRIX& parentGlobalTransform
) {
    if (!modelNode || !mCachedModelComponent) { // pCurrentAnim can be null
        return;
    }
    std::string nodeName = modelNode->GetName();
    DirectX::XMMATRIX nodeLocalAnimatedTransform;

    if (pCurrentAnim) { // If an animation is playing
        if (nodeName == "hip.L") {
            auto channelMapEntry = pCurrentAnim->channelMap.find(nodeName);
            if (channelMapEntry != pCurrentAnim->channelMap.end()) {
                const AnimationChannel& animChannel = pCurrentAnim->channels[channelMapEntry->second];
                DirectX::XMMATRIX s = InterpolateScaling(mCurrentTimeTicks, animChannel);
                DirectX::XMMATRIX r = InterpolateRotation(mCurrentTimeTicks, animChannel);
                DirectX::XMMATRIX t = InterpolatePosition(mCurrentTimeTicks, animChannel);
                nodeLocalAnimatedTransform = s * r * t;
            
            
                std::ostringstream oss_srt_hipL;
                oss_srt_hipL << "\n--- Bone (AnimComp): " << nodeName << " INTERPOLATED SRT ---\n";
                OutputDebugStringA(oss_srt_hipL.str().c_str());
                PrintMatrix("ScalingMatrix (hip.L anim)", s);
                PrintMatrix("RotationMatrix (hip.L anim)", r);
                PrintMatrix("TranslationMatrix (hip.L anim)", t);

            }
            else { // Node not in this animation, use bind pose
                nodeLocalAnimatedTransform = DirectX::XMLoadFloat4x4(&modelNode->GetOriginalTransform_ForAnim());
            }
        }
        else { // All other bones use bind pose
            nodeLocalAnimatedTransform = DirectX::XMLoadFloat4x4(&modelNode->GetOriginalTransform_ForAnim());
        }
    }
    else { // No animation playing (pCurrentAnim is nullptr), use bind pose
        nodeLocalAnimatedTransform = DirectX::XMLoadFloat4x4(&modelNode->GetOriginalTransform_ForAnim());
    }

    DirectX::XMMATRIX globalNodeTransform = nodeLocalAnimatedTransform * parentGlobalTransform;

    const auto& boneInfoMap = mCachedModelComponent->GetBoneInfoMap();
    auto boneInfoIt = boneInfoMap.find(nodeName);

    if (boneInfoIt != boneInfoMap.end()) {
        const BoneInfo& boneInfo = boneInfoIt->second;
        if (boneInfo.id >= 0 && static_cast<size_t>(boneInfo.id) < mFinalBoneMatrices.size()) {

            // CRITICAL DEBUG: Print the XMFLOAT4X4 offsetMatrix directly from boneInfo
            if (nodeName == "hip.L") { // Or your chosen debug bone name
                std::ostringstream oss_offset_check;
                oss_offset_check << "--- Bone: " << nodeName << " (ID: " << boneInfo.id << ") --- AnimComp READ Check (T-Pose or Anim)\n";
                oss_offset_check << "OffsetMatrix (XMFLOAT4X4 direct from boneInfo) in AnimComp:\n";
                const DirectX::XMFLOAT4X4& boffset = boneInfo.offsetMatrix;
                oss_offset_check << std::fixed << std::setprecision(3)
                    << "[" << boffset._11 << ", " << boffset._12 << ", " << boffset._13 << ", " << boffset._14 << "]\n"
                    << "[" << boffset._21 << ", " << boffset._22 << ", " << boffset._23 << ", " << boffset._24 << "]\n"
                    << "[" << boffset._31 << ", " << boffset._32 << ", " << boffset._33 << ", " << boffset._34 << "]\n"
                    << "[" << boffset._41 << ", " << boffset._42 << ", " << boffset._43 << ", " << boffset._44 << "]\n\n";
                OutputDebugStringA(oss_offset_check.str().c_str());
            }

            DirectX::XMMATRIX offsetMatrix = DirectX::XMLoadFloat4x4(&boneInfo.offsetMatrix);
            DirectX::XMMATRIX finalMatrix = offsetMatrix * globalNodeTransform;
            DirectX::XMStoreFloat4x4(&mFinalBoneMatrices[boneInfo.id], DirectX::XMMatrixTranspose(finalMatrix));

            // Your conditional PrintMatrix calls for detailed breakdown
            if (nodeName == "root" || nodeName == "centre") { // Example
                std::ostringstream oss_debug_anim; // Different stream to avoid conflict
                oss_debug_anim << "\n--- Bone (AnimComp): " << nodeName << " (ID: " << boneInfo.id << ") at t=" << mCurrentTimeTicks << " ---\n";
                OutputDebugStringA(oss_debug_anim.str().c_str());
                PrintMatrix("1. LocalSRT (Bind or Anim)", nodeLocalAnimatedTransform); // Renamed for clarity
                PrintMatrix("2. ParentGlobal", parentGlobalTransform);
                PrintMatrix("3. GlobalTransform (Local*Parent)", globalNodeTransform); // Renamed
                PrintMatrix("4. OffsetMatrix (Loaded from BoneInfo)", offsetMatrix); // Renamed
                PrintMatrix("5. FinalSkinningMatrix (Offset*Global)", finalMatrix); // Renamed
                PrintMatrix("6. mFinalBoneMatrices[id] (To Shader)", DirectX::XMLoadFloat4x4(&mFinalBoneMatrices[boneInfo.id]));
            }
        }
    }

    for (const auto& childNodeUPtr : modelNode->GetChildren_ForAnim()) {
        if (childNodeUPtr) {
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
