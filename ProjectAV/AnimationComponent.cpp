#include "AnimationComponent.h"
#include "Graphics.h"
#include "ModelComponent.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <functional>
#include "Bindable.h"
#include "ConstantBuffers.h"
namespace dx = DirectX;

SkeletalAnimationComponent::SkeletalAnimationComponent(Node* owner, Graphics& gfx, ModelComponent& model, const std::string& animationFile)
    : Component(owner), model(model)
{
    Assimp::Importer importer;
    pScene = importer.ReadFile(animationFile,
        aiProcess_Triangulate |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace);

    if (!pScene || !pScene->mRootNode || (pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE))
        throw std::runtime_error("Failed to load animation file: " + animationFile);

    // 1) load bone offsets + assign indices
    LoadBones(*pScene);

    // 2) load animation clips
    LoadAnimations(*pScene);

    // 3) pick the first clip as default (or specify by name)
    if (!animations.empty()) {
        currentAnimation = animations.begin()->second.get();
    }

    // 4) allocate the final-transforms vector & constant buffer
    finalBoneTransforms.assign(numBones, DirectX::XMFLOAT4X4{} /* identity */);
    pBoneCB = std::make_shared<Bind::VertexConstantBuffer<Bind::Bindable::BoneTransformCB>>(
        gfx, finalBoneTransforms.data(), numBones);

    // do an initial pose pass
    DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
    ReadNodeHierarchy(currentTime, pScene->mRootNode, identity);
}

void SkeletalAnimationComponent::Update(float dt)
{
    if (!isPlaying || !currentAnimation || !pScene)
        return;

    currentTime += dt * animationSpeed;
    float duration = static_cast<float>(currentAnimation->mDuration);

    if (currentTime > duration)
    {
        if (isLooping)
        {
            currentTime = 0.0f;
        }
        else
        {
            currentTime = duration;
            isPlaying = false;
        }
    }

    dx::XMMATRIX identity = dx::XMMatrixIdentity();
    ReadNodeHierarchy(currentTime, pScene->mRootNode, identity);
}

void SkeletalAnimationComponent::Draw(Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const
{
    if (currentAnimation)
    {
        // Get bone transforms from animation system
        auto& boneTransforms = GetFinalBoneTransforms();

        // Update bone transform constant buffer
        if (auto pcb = dynamic_cast<Bind::VertexConstantBuffer<Bind::Bindable::BoneTransformCB>*>(pBoneCB.get()))
        {
            Bind::Bindable::BoneTransformCB cb;
            for (int i = 0; i < boneTransforms.size(); i++)
            {
                dx::XMMATRIX matrix = dx::XMLoadFloat4x4(&boneTransforms[i]);
                dx::XMStoreFloat4x4(&cb.boneTransforms[i], matrix);
            }

            pcb->Update(gfx, cb);
        }

        // Bind resources
        pBoneCB->Bind(gfx);
    }

//    model.Submit(gfx, accumulatedTransform);
}

void SkeletalAnimationComponent::PlayAnimation(const std::string& name)
{
    auto it = animations.find(name);
    if (it != animations.end())
    {
        currentAnimation = it->second.get();
        currentTime = 0.0f;
        isPlaying = true;
    }
}

void SkeletalAnimationComponent::PauseAnimation()
{
    isPlaying = false;
}

void SkeletalAnimationComponent::ResumeAnimation()
{
    isPlaying = true;
}

void SkeletalAnimationComponent::SetAnimationSpeed(float speed)
{
    animationSpeed = speed;
}

void SkeletalAnimationComponent::SetLooping(bool loop)
{
    isLooping = loop;
}

void SkeletalAnimationComponent::LoadBones(const aiScene& scene)
{
    for (unsigned int i = 0; i < scene.mNumMeshes; i++)
    {
        const aiMesh* mesh = scene.mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumBones; j++)
        {
            const aiBone* bone = mesh->mBones[j];
            std::string boneName(bone->mName.data);

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                BoneInfo info;
                // assign this bone’s index in the final transform array:
                info.boneIndex = numBones;

                // store offset matrix (inverse bind-pose)
                dx::XMStoreFloat4x4(&info.offsetMatrix,
                    dx::XMMatrixTranspose(
                        dx::XMLoadFloat4x4(
                            reinterpret_cast<const dx::XMFLOAT4X4*>(&bone->mOffsetMatrix)
                        )
                    )
                );

                boneInfoMap[boneName] = info;
                numBones++;
            }
        }
    }
}


void SkeletalAnimationComponent::LoadAnimations(const aiScene& scene)
{
    for (unsigned int i = 0; i < scene.mNumAnimations; i++)
    {
        const aiAnimation* anim = scene.mAnimations[i];
        std::string animName(anim->mName.data);
        // wrap in shared_ptr only if you’re copying; otherwise you can store raw pointers
        animations[animName] = std::shared_ptr<const aiAnimation>(anim, [](auto) {/*no-op deleter*/});
    }
}


void SkeletalAnimationComponent::ReadNodeHierarchy(float animationTime, const aiNode* node, const dx::XMMATRIX& parentTransform)
{
    std::string nodeName(node->mName.data);
    dx::XMMATRIX nodeTransform = dx::XMLoadFloat4x4(
        reinterpret_cast<const dx::XMFLOAT4X4*>(&node->mTransformation));

    const aiNodeAnim* nodeAnim = FindNodeAnim(currentAnimation, nodeName);

    if (nodeAnim)
    {
        dx::XMMATRIX translation = InterpolatePosition(animationTime, nodeAnim);
        dx::XMMATRIX rotation = InterpolateRotation(animationTime, nodeAnim);
        dx::XMMATRIX scaling = InterpolateScaling(animationTime, nodeAnim);

        nodeTransform = scaling * rotation * translation;
    }

    dx::XMMATRIX globalTransform = nodeTransform * parentTransform;

    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        BoneInfo& boneInfo = boneInfoMap[nodeName];
        dx::XMMATRIX finalTransform =
            dx::XMLoadFloat4x4(&boneInfo.offsetMatrix) *
            globalTransform;

        dx::XMStoreFloat4x4(&boneInfo.finalTransform, finalTransform);
        dx::XMStoreFloat4x4(&finalBoneTransforms[boneInfo.boneIndex], finalTransform);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ReadNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
    }
}


const aiNodeAnim* SkeletalAnimationComponent::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
{
    for (unsigned int i = 0; i < animation->mNumChannels; i++)
    {
        const aiNodeAnim* nodeAnim = animation->mChannels[i];
        if (std::string(nodeAnim->mNodeName.data) == nodeName)
        {
            return nodeAnim;
        }
    }
    return nullptr;
}

dx::XMMATRIX SkeletalAnimationComponent::InterpolatePosition(float animationTime, const aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumPositionKeys == 1)
    {
        return dx::XMMatrixTranslation(
            nodeAnim->mPositionKeys[0].mValue.x,
            nodeAnim->mPositionKeys[0].mValue.y,
            nodeAnim->mPositionKeys[0].mValue.z);
    }

    unsigned int positionIndex = FindPosition(animationTime, nodeAnim);
    unsigned int nextPositionIndex = (positionIndex + 1);
    float deltaTime = static_cast<float>(nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
    float factor = static_cast<float>((animationTime - nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime);

    const aiVector3D& start = nodeAnim->mPositionKeys[positionIndex].mValue;
    const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIndex].mValue;
    aiVector3D delta = end - start;
    aiVector3D interpolated = start + factor * delta;

    return dx::XMMatrixTranslation(interpolated.x, interpolated.y, interpolated.z);
}

dx::XMMATRIX SkeletalAnimationComponent::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumRotationKeys == 1)
    {
        return dx::XMMatrixRotationQuaternion(dx::XMVectorSet(
            nodeAnim->mRotationKeys[0].mValue.x,
            nodeAnim->mRotationKeys[0].mValue.y,
            nodeAnim->mRotationKeys[0].mValue.z,
            nodeAnim->mRotationKeys[0].mValue.w));
    }

    unsigned int rotationIndex = FindRotation(animationTime, nodeAnim);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    float deltaTime = static_cast<float>(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
    float factor = static_cast<float>((animationTime - nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime);

    const aiQuaternion& start = nodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& end = nodeAnim->mRotationKeys[nextRotationIndex].mValue;
    aiQuaternion interpolated;
    aiQuaternion::Interpolate(interpolated, start, end, factor);
    interpolated.Normalize();

    return dx::XMMatrixRotationQuaternion(dx::XMVectorSet(
        interpolated.x, interpolated.y, interpolated.z, interpolated.w));
}

dx::XMMATRIX SkeletalAnimationComponent::InterpolateScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
    if (nodeAnim->mNumScalingKeys == 1)
    {
        return dx::XMMatrixScaling(
            nodeAnim->mScalingKeys[0].mValue.x,
            nodeAnim->mScalingKeys[0].mValue.y,
            nodeAnim->mScalingKeys[0].mValue.z);
    }

    unsigned int scalingIndex = FindScaling(animationTime, nodeAnim);
    unsigned int nextScalingIndex = (scalingIndex + 1);
    float deltaTime = static_cast<float>(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
    float factor = static_cast<float>((animationTime - nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime);

    const aiVector3D& start = nodeAnim->mScalingKeys[scalingIndex].mValue;
    const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIndex].mValue;
    aiVector3D delta = end - start;
    aiVector3D interpolated = start + factor * delta;

    return dx::XMMatrixScaling(interpolated.x, interpolated.y, interpolated.z);
}

unsigned int SkeletalAnimationComponent::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
    {
        if (animationTime < static_cast<float>(nodeAnim->mPositionKeys[i + 1].mTime))
        {
            return i;
        }
    }
    return 0;
}

unsigned int SkeletalAnimationComponent::FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
    {
        if (animationTime < static_cast<float>(nodeAnim->mRotationKeys[i + 1].mTime))
        {
            return i;
        }
    }
    return 0;
}

unsigned int SkeletalAnimationComponent::FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
    {
        if (animationTime < static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime))
        {
            return i;
        }
    }
    return 0;
}