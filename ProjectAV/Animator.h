#pragma once

#include "Animation.h"
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm> 
#include <cmath>   


class Animator
{
public:
     Animator(Animation* animation, bool initialLoop = true) :
        m_CurrentAnimation(animation),
        m_CurrentTime(0.0f),
        m_bLoopCurrentAnimation(initialLoop),  
        m_TargetAnimation(nullptr),
        m_TargetTime(0.0f),
        m_bLoopTargetAnimation(true), 
        m_BlendFactor(0.0f),
        m_TransitionDuration(0.0f),
        m_DeltaTime(0.0f)  
    {
        m_FinalBoneMatrices.reserve(150);
        for (int i = 0; i < 150; i++)
            m_FinalBoneMatrices.push_back(DirectX::XMMatrixIdentity());
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;

        if (!m_CurrentAnimation)
        {
            return;
        }
         
        if (m_CurrentAnimation->GetTicksPerSecond() > 0.0f && m_CurrentAnimation->GetDuration() > 0.0f)
        {
            m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
            if (m_bLoopCurrentAnimation)
            {
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            }
            else
            {
                if (m_CurrentTime >= m_CurrentAnimation->GetDuration())
                {
                    // ZMIANA START: Logika ustawiania animacji na nullptr po zakoñczeniu

                    // Ustaw czas na sam koniec, aby model pozosta³ w ostatniej klatce
                    m_CurrentTime = m_CurrentAnimation->GetDuration();

                    // Jeœli nie ma przejœcia do innej animacji, mo¿emy bezpiecznie "wy³¹czyæ" bie¿¹c¹
                    if (m_TargetAnimation == nullptr)
                    {
                        m_CurrentAnimation = nullptr;
                        // Opcjonalnie zresetuj czas, dla czystoœci kodu
                        m_CurrentTime = 0.0f;
                        // WyjdŸ z funkcji, nie ma potrzeby dalszego przetwarzania w tej klatce
                        return;
                    }

                    // ZMIANA KONIEC
                }
            }
        }
        else
        { 
            if (m_CurrentAnimation->GetDuration() <= 0.0f) m_CurrentTime = 0.0f;
        } 
        if (m_TargetAnimation)
        {
            if (m_TargetAnimation->GetTicksPerSecond() > 0.0f && m_TargetAnimation->GetDuration() > 0.0f)
            {
                m_TargetTime += m_TargetAnimation->GetTicksPerSecond() * dt;
                 if (m_bLoopTargetAnimation)
                {
                    m_TargetTime = fmod(m_TargetTime, m_TargetAnimation->GetDuration());
                }
                else
                {
                    if (m_TargetTime >= m_TargetAnimation->GetDuration())
                    {
                        m_TargetTime = m_TargetAnimation->GetDuration();
                    }
                }
            }
            else
            {
                if (m_TargetAnimation->GetDuration() <= 0.0f) m_TargetTime = 0.0f;
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
                m_bLoopCurrentAnimation = m_bLoopTargetAnimation; 

                m_TargetAnimation = nullptr;
                m_TargetTime = 0.0f;
                m_bLoopTargetAnimation = true; 
                m_BlendFactor = 0.0f;
                m_TransitionDuration = 0.0f;
            }
        }
          
        DirectX::XMMATRIX identityMatrix = DirectX::XMMatrixIdentity();
        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), identityMatrix);
 
    }
     
    void PlayAnimation(Animation* pAnimation, float transitionDuration = 0.5f, bool loop = true)
    {
        if (!pAnimation)
        { 
            return;
        }

        // Przypadek 1: Ta sama animacja jest ju¿ bie¿¹ca I NIE ma przejœcia w toku
        if (pAnimation == m_CurrentAnimation && !m_TargetAnimation)
        {
            if (m_bLoopCurrentAnimation != loop)  
            {
                m_bLoopCurrentAnimation = loop; 
                if (loop && m_CurrentAnimation->GetDuration() > 0.0f && m_CurrentTime >= m_CurrentAnimation->GetDuration()) {
                    m_CurrentTime = 0.0f;
                }
            }
            return;
        }

        // Przypadek 2: Animacja jest ju¿ ustawiona jako docelowa
        if (pAnimation == m_TargetAnimation)
        {
            m_bLoopTargetAnimation = loop; 
            return;
        }

        // G³ówna logika ustawiania animacji
        if (!m_CurrentAnimation || transitionDuration <= 0.0f)  
        {
            m_CurrentAnimation = pAnimation;
            m_CurrentTime = 0.0f;
            m_bLoopCurrentAnimation = loop;

            m_TargetAnimation = nullptr;
            m_TargetTime = 0.0f;
            m_bLoopTargetAnimation = true; 
            m_BlendFactor = 0.0f;
            m_TransitionDuration = 0.0f;
        }
        else 
        {
            m_TargetAnimation = pAnimation;
            m_TargetTime = 0.0f;  
            m_bLoopTargetAnimation = loop;

            m_BlendFactor = 0.0f;
            m_TransitionDuration = transitionDuration;
        }
    }

    void CalculateBoneTransform(const AssimpNodeData* node, DirectX::XMMATRIX& parentTransform)
    {
        std::string nodeName = node->name;
        DirectX::XMMATRIX nodeTransform = node->transformation;  

        Bone* currentBone = nullptr;
        if (m_CurrentAnimation)  
        {
            currentBone = m_CurrentAnimation->FindBone(nodeName);
            if (currentBone)
            {
                currentBone->Update(m_CurrentTime);
                nodeTransform = currentBone->GetLocalTransform();
            }
        }

        DirectX::XMMATRIX blendedTransform = nodeTransform; 

        if (m_TargetAnimation && m_BlendFactor > 0.0f)
        {
            DirectX::XMMATRIX targetNodeTransform = node->transformation;  
            Bone* targetBone = m_TargetAnimation->FindBone(nodeName);

            if (targetBone)
            {
                targetBone->Update(m_TargetTime);
                targetNodeTransform = targetBone->GetLocalTransform();
            }
             
            DirectX::XMVECTOR scaleA, rotQuatA, transA;
            DirectX::XMMatrixDecompose(&scaleA, &rotQuatA, &transA, nodeTransform);  

            DirectX::XMVECTOR scaleB, rotQuatB, transB;
            DirectX::XMMatrixDecompose(&scaleB, &rotQuatB, &transB, targetNodeTransform);

            DirectX::XMVECTOR blendedScale = DirectX::XMVectorLerp(scaleA, scaleB, m_BlendFactor);
            DirectX::XMVECTOR blendedRotQuat = DirectX::XMQuaternionSlerp(rotQuatA, rotQuatB, m_BlendFactor);
            DirectX::XMVECTOR blendedTrans = DirectX::XMVectorLerp(transA, transB, m_BlendFactor);

            blendedTransform = DirectX::XMMatrixScalingFromVector(blendedScale) *
                DirectX::XMMatrixRotationQuaternion(blendedRotQuat) *
                DirectX::XMMatrixTranslationFromVector(blendedTrans);
        }

        DirectX::XMMATRIX globalTransformation = blendedTransform * parentTransform;

        if (m_CurrentAnimation) 
        {
            std::map<std::string, ModelComponent::BoneInfo>& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
            auto it = boneInfoMap.find(nodeName);
            if (it != boneInfoMap.end())
            {
                int boneID = it->second.id;
                DirectX::XMMATRIX offsetMatrix = it->second.offset;
                if (boneID >= 0 && static_cast<size_t>(boneID) < m_FinalBoneMatrices.size())
                {
                    m_FinalBoneMatrices[boneID] = offsetMatrix * globalTransformation;
                }
            }
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<DirectX::XMMATRIX> GetFinalBoneMatrices() const
    {
        return m_FinalBoneMatrices;
    }

    Animation* GetCurrentAnimation() const {
        return m_CurrentAnimation;
    }

    bool IsCurrentAnimationLooping() const {
        if (m_TargetAnimation && m_BlendFactor < 1.0f) {  
            return m_bLoopTargetAnimation; 
        }
        return m_bLoopCurrentAnimation;
    }


private:
    std::vector<DirectX::XMMATRIX> m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    bool  m_bLoopCurrentAnimation;  

    Animation* m_TargetAnimation;
    float m_TargetTime;
    bool  m_bLoopTargetAnimation;  

    float m_BlendFactor;
    float m_TransitionDuration;
    float m_DeltaTime;  
};