#pragma once
#include <map>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "ModelComponent.h" // <-- Potrzebujemy definicji ModelComponent::BoneInfo

// Struktura, któr¹ bêdzie zwracaæ nasz cache. Zawiera wszystko, czego potrzeba.
struct CachedModelData
{
    const aiScene* pScene = nullptr;
    std::map<std::string, ModelComponent::BoneInfo> BoneInfoMap;
    int BoneCounter = 0;
};

class ModelCache
{
public:
    static ModelCache& Get();

    // Zmieniamy funkcjê, by zwraca³a nasz¹ now¹ strukturê
    const CachedModelData& LoadOrGet(const std::string& filePath);

private:
    ModelCache() = default;
    ~ModelCache() = default;
    ModelCache(const ModelCache&) = delete;
    ModelCache& operator=(const ModelCache&) = delete;

    // Wewnêtrzna struktura przechowuj¹ca dane. Teraz zawiera te¿ info o koœciach.
    struct SceneData
    {
        std::unique_ptr<Assimp::Importer> importer;
        CachedModelData cachedData;
    };

    std::map<std::string, SceneData> m_cache;
};