#pragma once

#include "Animation.h"
#include <DirectXMath.h>

class Animator
{
public:
    Animator(Animation* Animation)
    {
        m_CurrentTime = 0.0;
        m_CurrentAnimation = Animation;

        m_FinalBoneMatrices.reserve(100);

        for (int i = 0; i < 100; i++)
            m_FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), identityMatrix);
        }
    }

    void PlayAnimation(Animation* pAnimation)
    {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }

    void CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX& parentTransform)
    {
        std::string nodeName = node->name;
        DirectX::XMMATRIX nodeTransform = node->transformation;

        Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

        if (Bone)
        {
            Bone->Update(m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }
        
        DirectX::XMMATRIX globalTransformation = nodeTransform * parentTransform;

        std::map<std::string, ModelComponent::BoneInfo>& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            DirectX::XMMATRIX offset = boneInfoMap[nodeName].offset;
            m_FinalBoneMatrices[index] = offset * globalTransformation;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<DirectX::XMMATRIX> GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }

private:
    std::vector<DirectX::XMMATRIX> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
};