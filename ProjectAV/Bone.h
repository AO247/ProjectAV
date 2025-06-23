#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <assimp/anim.h>
#include "AssimpHelperFunctions.h"

struct KeyPosition
{
    DirectX::XMFLOAT3 position;
    float timeStamp;
};

struct KeyRotation
{
    DirectX::XMFLOAT4 orientation;
    float timeStamp;
};

struct KeyScale
{
    DirectX::XMFLOAT3 scale;
    float timeStamp;
};

class Bone
{
private:
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    int m_NumPositions;
    int m_NumRotations;
    int m_NumScalings;

    DirectX::XMMATRIX m_LocalTransform;
    std::string m_Name;
    int m_ID;



public:

    /*reads keyframes from aiNodeAnim*/
    Bone(const std::string& name, int ID, const aiNodeAnim* channel)
        :
        m_Name(name),
        m_ID(ID),
        m_LocalTransform(DirectX::XMMatrixIdentity())
    {
        m_NumPositions = channel->mNumPositionKeys;

        for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = GetDXVec(aiPosition);
            data.timeStamp = timeStamp;
            m_Positions.push_back(data);
        }

        m_NumRotations = channel->mNumRotationKeys;
        for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = GetDXQuat(aiOrientation);
            data.timeStamp = timeStamp;
            m_Rotations.push_back(data);
        }

        m_NumScalings = channel->mNumScalingKeys;
        for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = GetDXVec(scale);
            data.timeStamp = timeStamp;
            m_Scales.push_back(data);
        }
    }

    //interpolates  positions,rotations & scaling keys based on the curren time of
    //the animation and prepares the local transformation matrix by combining all keys
    //tranformations
    void Update(float animationTime)
    {
        DirectX::XMMATRIX translation = InterpolatePosition(animationTime);
        DirectX::XMMATRIX rotation = InterpolateRotation(animationTime);
        DirectX::XMMATRIX scale = InterpolateScaling(animationTime);
        m_LocalTransform = scale * rotation * translation;
    }

    DirectX::XMMATRIX GetLocalTransform() { return m_LocalTransform; }
    std::string GetBoneName() const { return m_Name; }
    int GetBoneID() { return m_ID; }


    // Gets the current index on mKeyPositions to interpolate to based on
    //the current animation time
    int GetPositionIndex(float animationTime)
    { 
        for (int index = 0; index < m_NumPositions - 1; ++index)
        {
            if (animationTime < m_Positions[index + 1].timeStamp)
                return index;
        } 
        return m_NumPositions - 2;  
    }

    int GetRotationIndex(float animationTime)
    { 
        for (int index = 0; index < m_NumRotations - 1; ++index)
        {
            if (animationTime < m_Rotations[index + 1].timeStamp)
                return index;
        }
        return m_NumRotations - 2;  
    }

    int GetScaleIndex(float animationTime)
    { 
        for (int index = 0; index < m_NumScalings - 1; ++index)
        {
            if (animationTime < m_Scales[index + 1].timeStamp)
                return index;
        }
        return m_NumScalings - 2;  
    }

private:

    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
    {
        float scaleFactor = 0.0f;
        float midWayLength = animationTime - lastTimeStamp;
        float framesDiff = nextTimeStamp - lastTimeStamp; 
        if (framesDiff == 0.0f)
            return 0.0f; 
        scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }

    DirectX::XMMATRIX InterpolatePosition(float animationTime)
    {
        // Przypadek 1: Brak klatek kluczowych pozycji
        if (m_NumPositions == 0)
            return DirectX::XMMatrixIdentity();  

        // Przypadek 2: Jedna klatka kluczowa pozycji
        if (m_NumPositions == 1)
            return DirectX::XMMatrixTranslation(m_Positions[0].position.x, m_Positions[0].position.y, m_Positions[0].position.z);

        // Przypadek 3: Dwie lub wiêcej klatek kluczowych (GetPositionIndex jest teraz bezpieczne)
        int p0Index = GetPositionIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp,
            m_Positions[p1Index].timeStamp, animationTime);

        DirectX::XMVECTOR pos0 = XMLoadFloat3(&m_Positions[p0Index].position);
        DirectX::XMVECTOR pos1 = XMLoadFloat3(&m_Positions[p1Index].position);
        DirectX::XMVECTOR finalPositionVec = DirectX::XMVectorLerp(pos0, pos1, scaleFactor);
        DirectX::XMFLOAT3 finalPosition;
        DirectX::XMStoreFloat3(&finalPosition, finalPositionVec);

        return DirectX::XMMatrixTranslation(finalPosition.x, finalPosition.y, finalPosition.z);
    }

    DirectX::XMMATRIX InterpolateRotation(float animationTime)
    {
        // Przypadek 1: Brak klatek kluczowych rotacji
        if (m_NumRotations == 0)
            return DirectX::XMMatrixIdentity(); 

        // Przypadek 2: Jedna klatka kluczowa rotacji
        if (m_NumRotations == 1)
        {
            DirectX::XMVECTOR v = DirectX::XMLoadFloat4(&m_Rotations[0].orientation); 
            DirectX::XMVECTOR vNormalized = DirectX::XMQuaternionNormalize(v);
            return DirectX::XMMatrixRotationQuaternion(vNormalized);
        }

        // Przypadek 3: Dwie lub wiêcej klatek kluczowych
        int p0Index = GetRotationIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp,
            m_Rotations[p1Index].timeStamp, animationTime);

        DirectX::XMVECTOR quat0 = DirectX::XMLoadFloat4(&m_Rotations[p0Index].orientation);
        DirectX::XMVECTOR quat1 = DirectX::XMLoadFloat4(&m_Rotations[p1Index].orientation); 
        DirectX::XMVECTOR finalRotation = DirectX::XMQuaternionSlerp(quat0, quat1, scaleFactor);
        finalRotation = DirectX::XMQuaternionNormalize(finalRotation);  

        return DirectX::XMMatrixRotationQuaternion(finalRotation);
    }

    DirectX::XMMATRIX InterpolateScaling(float animationTime)
    {
        // Przypadek 1: Brak klatek kluczowych skali
        if (m_NumScalings == 0)
            return DirectX::XMMatrixIdentity();  

        // Przypadek 2: Jedna klatka kluczowa skali
        if (m_NumScalings == 1)
            return DirectX::XMMatrixScaling(m_Scales[0].scale.x, m_Scales[0].scale.y, m_Scales[0].scale.z);

        // Przypadek 3: Dwie lub wiêcej klatek kluczowych
        int p0Index = GetScaleIndex(animationTime);
        int p1Index = p0Index + 1;
        float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp,
            m_Scales[p1Index].timeStamp, animationTime);

        DirectX::XMVECTOR scaleVec0 = XMLoadFloat3(&m_Scales[p0Index].scale);
        DirectX::XMVECTOR scaleVec1 = XMLoadFloat3(&m_Scales[p1Index].scale);
        DirectX::XMVECTOR finalScaleVec = DirectX::XMVectorLerp(scaleVec0, scaleVec1, scaleFactor);
        DirectX::XMFLOAT3 finalScale;
        DirectX::XMStoreFloat3(&finalScale, finalScaleVec);

        return DirectX::XMMatrixScaling(finalScale.x, finalScale.y, finalScale.z);
    }

};