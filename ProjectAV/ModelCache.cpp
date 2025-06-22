#include "ModelCache.h"
#include "ModelException.h"
#include <assimp/postprocess.h>

ModelCache& ModelCache::Get()
{
    static ModelCache instance;
    return instance;
}

// Prywatna funkcja pomocnicza do ekstrakcji koœci (przeniesiona z ModelComponent)
void ExtractBoneInfo(const aiScene& scene, std::map<std::string, ModelComponent::BoneInfo>& boneInfoMap, int& boneCounter)
{
    boneCounter = 0;
    boneInfoMap.clear();

    for (unsigned int meshIdx = 0; meshIdx < scene.mNumMeshes; ++meshIdx)
    {
        const auto& mesh = *scene.mMeshes[meshIdx];
        if (!mesh.HasBones())
        {
            continue;
        }

        for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx)
        {
            const auto& bone = *mesh.mBones[boneIdx];
            std::string boneName = bone.mName.C_Str();

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                ModelComponent::BoneInfo newBoneInfo;
                newBoneInfo.id = boneCounter;
                newBoneInfo.offset = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
                    reinterpret_cast<const DirectX::XMFLOAT4X4*>(&bone.mOffsetMatrix)
                ));
                boneInfoMap[boneName] = newBoneInfo;
                boneCounter++;
            }
        }
    }
}

const CachedModelData& ModelCache::LoadOrGet(const std::string& filePath)
{
    auto it = m_cache.find(filePath);
    if (it != m_cache.end())
    {
        return it->second.cachedData;
    }

    OutputDebugStringA(("ModelCache: Loading AND processing new model from file: " + filePath + "\n").c_str());

    SceneData data;
    data.importer = std::make_unique<Assimp::Importer>();

    const auto pScene = data.importer->ReadFile(filePath.c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_ConvertToLeftHanded |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace
    );

    if (!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        throw ModelException(__LINE__, __FILE__, "Assimp error loading file " + filePath + ": " + std::string(data.importer->GetErrorString()));
    }

    // Wype³nij nasz¹ strukturê do cache'owania
    data.cachedData.pScene = pScene;

    // Uruchom logikê ekstrakcji koœci TUTAJ, tylko raz!
    ExtractBoneInfo(*pScene, data.cachedData.BoneInfoMap, data.cachedData.BoneCounter);

    // Przenieœ dane do mapy cache'u
    auto& insertedData = m_cache[filePath] = std::move(data);

    // Zwróæ referencjê do w³aœnie dodanych danych
    return insertedData.cachedData;
}