#pragma once
#include "Component.h"
#include "ModelComponent.h"
#include <assimp/scene.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Bindable.h"
#include "ConstantBuffers.h"
#include "Bindable.h"

namespace DirectX
{
    struct XMMATRIX;
}

class Graphics;

class SkeletalAnimationComponent : public Component
{
public:
    struct BoneInfo
    {
        int boneIndex = -1;
        DirectX::XMFLOAT4X4 offsetMatrix;
        DirectX::XMFLOAT4X4 finalTransform;
    };

    SkeletalAnimationComponent(Node* owner, Graphics& gfx, ModelComponent& model, const std::string& animationFile);
    ~SkeletalAnimationComponent() noexcept override = default;

    void Update(float dt) override;
    void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const;

    void PlayAnimation(const std::string& name);
    void PauseAnimation();
    void ResumeAnimation();
    void SetAnimationSpeed(float speed);
    void SetLooping(bool loop);

    const std::vector<DirectX::XMFLOAT4X4>& GetFinalBoneTransforms() const { return finalBoneTransforms; }

private:
    void LoadBones(const aiScene& scene);
    void LoadAnimations(const aiScene& scene);
    void ReadNodeHierarchy(float animationTime, const aiNode* node, const DirectX::XMMATRIX& parentTransform);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);

    DirectX::XMMATRIX InterpolatePosition(float animationTime, const aiNodeAnim* nodeAnim);
    DirectX::XMMATRIX InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
    DirectX::XMMATRIX InterpolateScaling(float animationTime, const aiNodeAnim* nodeAnim);

    unsigned int FindPosition(float animationTime, const aiNodeAnim* nodeAnim);
    unsigned int FindRotation(float animationTime, const aiNodeAnim* nodeAnim);
    unsigned int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);

    ModelComponent& model;
    const aiScene* pScene = nullptr;
    std::unordered_map<std::string, BoneInfo> boneInfoMap;
    std::vector<DirectX::XMFLOAT4X4> finalBoneTransforms;
    std::unordered_map<std::string, std::shared_ptr<const aiAnimation>> animations;

    const aiAnimation* currentAnimation = nullptr;
    float currentTime = 0.0f;
    float animationSpeed = 1.0f;
    bool isPlaying = true;
    bool isLooping = true;
    int numBones = 0;

    std::shared_ptr<Bind::VertexConstantBuffer<Bind::Bindable::BoneTransformCB>> pBoneCB;

};