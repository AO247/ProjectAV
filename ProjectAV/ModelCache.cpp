//#include "ModelCache.h"
//#include "Graphics.h"
//#include "ModelException.h"
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>
//#include <filesystem>
//
//ModelCache& ModelCache::GetInstance()
//{
//    static ModelCache instance;
//    return instance;
//}
//
//std::shared_ptr<CachedModelData> ModelCache::GetData(Graphics& gfx, const std::string& modelFile, bool isSkinned)
//{
//    std::lock_guard<std::mutex> lock(mtx); // Zabezpieczenie dostêpu
//
//    // Kluczem mo¿e byæ kombinacja œcie¿ki i flagi isSkinned
//    std::string key = modelFile + (isSkinned ? "_skinned" : "_static");
//
//    auto it = cache.find(key);
//    if (it != cache.end())
//    {
//        // Znaleziono w cache, zwróæ istniej¹ce dane
//        return it->second;
//    }
//
//    // Nie znaleziono, wczytaj model
//    auto loadedData = LoadModel(gfx, modelFile, isSkinned);
//    cache[key] = loadedData;
//    return loadedData;
//}
//
//// Ta funkcja zawiera logikê przeniesion¹ z oryginalnego konstruktora ModelComponent
//std::shared_ptr<CachedModelData> ModelCache::LoadModel(Graphics& gfx, const std::string& modelFile, bool isSkinned)
//{
//    OutputDebugStringA(("Loading model for the first time: " + modelFile + "\n").c_str());
//
//    auto data = std::make_shared<CachedModelData>();
//    data->isSkinned = isSkinned;
//
//    Assimp::Importer importer;
//    const auto pScene = importer.ReadFile(modelFile.c_str(),
//        aiProcess_Triangulate |
//        aiProcess_JoinIdenticalVertices |
//        aiProcess_ConvertToLeftHanded |
//        aiProcess_GenNormals |
//        aiProcess_CalcTangentSpace
//    );
//
//    if (!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
//        throw ModelException(__LINE__, __FILE__, "Assimp error: " + std::string(importer.GetErrorString()));
//    }
//
//    // 1. Przetwarzanie koœci (jeœli model jest animowany)
//    if (isSkinned)
//    {
//        // Ta funkcja musi staæ siê czêœci¹ ModelCache lub byæ statyczna
//        // Na razie za³ó¿my, ¿e kopiujemy jej logikê tutaj.
//        for (unsigned int meshIdx = 0; meshIdx < pScene->mNumMeshes; ++meshIdx)
//        {
//            const auto& mesh = *pScene->mMeshes[meshIdx];
//            if (!mesh.HasBones()) continue;
//            for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx)
//            {
//                const auto& bone = *mesh.mBones[boneIdx];
//                std::string boneName = bone.mName.C_Str();
//                if (data->boneInfoMap.find(boneName) == data->boneInfoMap.end())
//                {
//                    ModelComponent::BoneInfo newBoneInfo;
//                    newBoneInfo.id = data->boneCounter;
//                    newBoneInfo.offset = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
//                        reinterpret_cast<const DirectX::XMFLOAT4X4*>(&bone.mOffsetMatrix)
//                    ));
//                    data->boneInfoMap[boneName] = newBoneInfo;
//                    data->boneCounter++;
//                }
//            }
//        }
//    }
//
//    // 2. Przetwarzanie materia³ów i meshy
//    std::filesystem::path filePath(modelFile);
//    std::vector<Material> materials;
//    materials.reserve(pScene->mNumMaterials);
//    for (size_t i = 0; i < pScene->mNumMaterials; ++i) {
//        materials.emplace_back(gfx, *pScene->mMaterials[i], filePath, isSkinned);
//    }
//
//    // Zmieniamy unique_ptr na shared_ptr
//    data->meshPtrs.reserve(pScene->mNumMeshes);
//    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
//        const auto& assimpMesh = *pScene->mMeshes[i];
//        data->meshPtrs.push_back(std::make_shared<Mesh>(gfx, materials[assimpMesh.mMaterialIndex], assimpMesh, 1.0f));
//    }
//
//    // 3. Parsowanie hierarchii wêz³ów
//    // To jest problematyczne, bo ParseNodeRecursive jest prywatne w ModelComponent.
//    // Musimy je przenieœæ lub uczyniæ dostêpnym. Najproœciej jest skopiowaæ jego logikê.
//
//    // Funkcja pomocnicza do rekursywnego parsowania wêz³ów
//    std::function<std::unique_ptr<Model
// 
// Node>(int&, const aiNode&)> parseFunc =
//        [&](int& nextId, const aiNode& node) -> std::unique_ptr<ModelInternalNode>
//        {
//            const auto transformMatrix = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(
//                reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)
//            ));
//
//            std::vector<Mesh*> currentNodeMeshPtrs;
//            currentNodeMeshPtrs.reserve(node.mNumMeshes);
//            for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
//                const auto meshIndex = node.mMeshes[i];
//                if (meshIndex < data->meshPtrs.size()) {
//                    currentNodeMeshPtrs.push_back(data->meshPtrs[meshIndex].get());
//                }
//            }
//
//            auto pNode = std::make_unique<ModelInternalNode>(nextId++, node.mName.C_Str(), std::move(currentNodeMeshPtrs), transformMatrix);
//
//            for (unsigned int i = 0; i < node.mNumChildren; ++i) {
//                pNode->AddChild(parseFunc(nextId, *node.mChildren[i]));
//            }
//            return pNode;
//        };
//
//    int nextId = 0;
//    data->rootNode = parseFunc(nextId, *pScene->mRootNode);
//
//    return data;
//}