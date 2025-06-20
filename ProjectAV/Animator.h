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

        if (!m_CurrentAnimation)
        {
            return;
        }

        if (m_CurrentAnimation->GetTicksPerSecond() > 0 && m_CurrentAnimation->GetDuration() > 0)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            //DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
            //CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), identityMatrix);
        }
        else
        {
            m_CurrentTime = 0.0f;
        }

        if (m_TargetAnimation)
        {
            if (m_TargetAnimation->GetTicksPerSecond() > 0 && m_TargetAnimation->GetDuration() > 0)
            {
                m_TargetTime += m_TargetAnimation->GetTicksPerSecond() * dt;
                m_TargetTime = fmod(m_TargetTime, m_TargetAnimation->GetDuration());
            }
            else
            {
                m_TargetTime = 0.0f;
            }

            if (m_TransitionDuration > 0.0f)
            {
                m_BlendFactor += dt / m_TransitionDuration;
            }
            else
            {
                m_BlendFactor = 1.0f;
            }
            m_BlendFactor = std::min(m_BlendFactor, 1.0f);

            if (m_BlendFactor >= 1.0f)
            {
                m_CurrentAnimation = m_TargetAnimation;
                m_CurrentTime = m_TargetTime;

                m_TargetAnimation = nullptr;
                m_TargetTime = 0.0f;
                m_BlendFactor = 0.0f;
            }

        }

        DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), identityMatrix);

    }

    void PlayAnimation(Animation* pAnimation, float transitionDuration = 0.2f)
    {
        if (!pAnimation || pAnimation == m_CurrentAnimation)
        {
            if (pAnimation == m_CurrentAnimation && !m_TargetAnimation) return;
            if (pAnimation == m_TargetAnimation) return;
        }

        if (!m_CurrentAnimation || transitionDuration <= 0.0f || pAnimation == m_CurrentAnimation)
        {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
            m_TargetAnimation = nullptr;
            m_BlendFactor = 0.0f;
        }
        else
        {
            m_TargetAnimation = pAnimation;
            m_TargetTime = 0.0f;
            m_BlendFactor = 0.0f;
            m_TransitionDuration = transitionDuration;
        }
    }

    void CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX& parentTransform)
    {
        std::string nodeName = node->name;
        DirectX::XMMATRIX nodeTransform = node->transformation;

        Bone* BoneC = m_CurrentAnimation->FindBone(nodeName);

        if (BoneC)
        {
            BoneC->Update(m_CurrentTime);
            nodeTransform = BoneC->GetLocalTransform();
        }

        DirectX::XMMATRIX nodeTransformTargetFinal = nodeTransform;
        if (m_TargetAnimation && m_BlendFactor > 0.0f)
        {
            DirectX::XMMATRIX nodeTransformTarget = node->transformation;
            Bone* boneTarget = m_TargetAnimation->FindBone(nodeName);

            if (boneTarget)
            {
                boneTarget->Update(m_TargetTime);
                nodeTransformTarget = boneTarget->GetLocalTransform();
            }

            DirectX::XMVECTOR scaleA, rotQuatA, transA;
            DirectX::XMMatrixDecompose(&scaleA, &rotQuatA, &transA, nodeTransform);

            DirectX::XMVECTOR scaleB, rotQuatB, transB;
            DirectX::XMMatrixDecompose(&scaleB, &rotQuatB, &transB, nodeTransformTarget);

            DirectX::XMVECTOR blendedScale = DirectX::XMVectorLerp(scaleA, scaleB, m_BlendFactor);
            DirectX::XMVECTOR blendedRotQuat = DirectX::XMQuaternionSlerp(rotQuatA, rotQuatB, m_BlendFactor);
            DirectX::XMVECTOR blendedTrans = DirectX::XMVectorLerp(transA, transB, m_BlendFactor);

            nodeTransformTargetFinal = DirectX::XMMatrixScalingFromVector(blendedScale) *
                DirectX::XMMatrixRotationQuaternion(blendedRotQuat) *
                DirectX::XMMatrixTranslationFromVector(blendedTrans);

        }


        DirectX::XMMATRIX globalTransformation = nodeTransformTargetFinal * parentTransform;

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

    Animation* GetCurrentAnimation() const {
        return m_CurrentAnimation;
    }

private:
    std::vector<DirectX::XMMATRIX> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;

    Animation* m_TargetAnimation;
    float m_TargetTime;
    float m_BlendFactor;
    float m_TransitionDuration;
};