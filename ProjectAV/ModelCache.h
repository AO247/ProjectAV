#pragma once
#include <map>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "ModelComponent.h" // <-- Potrzebujemy definicji ModelComponent::BoneInfo

// Struktura, kt�r� b�dzie zwraca� nasz cache. Zawiera wszystko, czego potrzeba.
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

    // Zmieniamy funkcj�, by zwraca�a nasz� now� struktur�
    const CachedModelData& LoadOrGet(const std::string& filePath);

private:
    ModelCache() = default;
    ~ModelCache() = default;
    ModelCache(const ModelCache&) = delete;
    ModelCache& operator=(const ModelCache&) = delete;

    // Wewn�trzna struktura przechowuj�ca dane. Teraz zawiera te� info o ko�ciach.
    struct SceneData
    {
        std::unique_ptr<Assimp::Importer> importer;
        CachedModelData cachedData;
    };

    std::map<std::string, SceneData> m_cache;
};