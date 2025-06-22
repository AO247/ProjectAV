#include "ModelComponent.h"
#include "Mesh.h" 
#include "ModelException.h"
#include "AnimationComponent.h"
#include "Node.h" 
#include "Graphics.h"
#include "Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <set>
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace dx = DirectX;

// Usuniêto klasy ModelControlWindow i ModelInternalNode oraz ich implementacje

ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale, bool isSkinned)
	: Component(owner),
	skinnedCharacter(isSkinned) // Inicjalizacja sk³adowej
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(modelFile.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		throw ModelException(__LINE__, __FILE__, "Assimp error: " + std::string(importer.GetErrorString()));
	}

	// Wyodrêbnij informacje o koœciach tylko, jeœli model jest oznaczony jako "skinned"
	if (skinnedCharacter)
	{
		ExtractBoneInfo(*pScene);
	}

	std::filesystem::path filePath(modelFile);

	// Za³aduj materia³y
	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);
	for (size_t i = 0; i < pScene->mNumMaterials; ++i) {
		// Przeka¿ flagê `skinnedCharacter` do konstruktora materia³u
		materials.emplace_back(gfx, *pScene->mMaterials[i], filePath, skinnedCharacter);
	}

	// Za³aduj wszystkie siatki (meshe) z modelu do p³askiej listy
	meshPtrs.reserve(pScene->mNumMeshes);
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		const auto& assimpMesh = *pScene->mMeshes[i];
		meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[assimpMesh.mMaterialIndex], assimpMesh, scale));
	}
}

void ModelComponent::ExtractBoneInfo(const aiScene& scene)
{
	// Przechodzi przez wszystkie siatki w scenie, aby znaleŸæ wszystkie unikalne koœci
	for (unsigned int meshIdx = 0; meshIdx < scene.mNumMeshes; ++meshIdx)
	{
		const auto& mesh = *scene.mMeshes[meshIdx];
		if (!mesh.HasBones())
		{
			continue;
		}

		// Przechodzi przez wszystkie koœci w bie¿¹cej siatce
		for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx)
		{
			const auto& bone = *mesh.mBones[boneIdx];
			std::string boneName = bone.mName.C_Str();

			// Jeœli nie widzieliœmy tej koœci wczeœniej, dodaj j¹ do naszej mapy
			if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = m_BoneCounter;
				// Macierz z Assimp musi zostaæ przekonwertowana i transponowana dla DirectX.
				// Ta macierz transformuje wierzcho³ki z przestrzeni modelu do lokalnej przestrzeni koœci.
				newBoneInfo.offset = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
					reinterpret_cast<const dx::XMFLOAT4X4*>(&bone.mOffsetMatrix)
				));
				m_BoneInfoMap[boneName] = newBoneInfo;
				m_BoneCounter++;
			}
		}
	}
}

void ModelComponent::Submit(Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd
{
	// SprawdŸ, czy istnieje komponent animacji, aby uzyskaæ dane o koœciach
	AnimationComponent* animationComponent = pOwner->GetComponent<AnimationComponent>();
	const std::vector<DirectX::XMMATRIX>* pBoneTransforms = nullptr;

	// Zmienna lokalna do przechowywania macierzy koœci. Musi istnieæ tak d³ugo, 
	// jak d³ugo wskaŸnik pBoneTransforms jest u¿ywany.
	std::vector<dx::XMMATRIX> boneMatrices;

	if (animationComponent != nullptr)
	{
		// 1. Pobierz macierze (jako kopiê) do naszej zmiennej lokalnej.
		boneMatrices = animationComponent->animator->GetFinalBoneMatrices();
		// 2. Teraz mo¿emy bezpiecznie pobraæ adres naszej zmiennej lokalnej (l-wartoœci).
		pBoneTransforms = &boneMatrices;
	}

	// PrzejdŸ przez wszystkie siatki w modelu i wyœlij je do renderowania
	for (const auto& pMesh : meshPtrs)
	{
		// Metoda Submit siatki obs³u¿y wskaŸnik pBoneTransforms (nawet jeœli jest to nullptr)
		pMesh->Submit(worldTransform, pBoneTransforms);
	}
}

std::vector<ModelComponent::Triangle> ModelComponent::GetAllTriangles() const
{
	std::vector<ModelComponent::Triangle> allTriangles;

	if (meshPtrs.empty()) {
		return allTriangles;
	}

	for (const auto& pMeshUniquePtr : meshPtrs)
	{
		if (!pMeshUniquePtr) {
			continue;
		}
		const Mesh* pMesh = pMeshUniquePtr.get();

		std::shared_ptr<const Bind::VertexBuffer> pBindVertexBuffer = pMesh->GetVertexBuffer();
		std::shared_ptr<const Bind::IndexBuffer> pBindIndexBuffer = pMesh->GetIndexBuffer();

		if (!pBindVertexBuffer || !pBindIndexBuffer) {
			continue;
		}

		const Dvtx::VertexLayout& layout = pBindVertexBuffer->GetLayout();
		bool hasPos3D = layout.Has(Dvtx::VertexLayout::Position3D);
		bool hasPos2D = layout.Has(Dvtx::VertexLayout::Position2D);

		if (!hasPos3D && !hasPos2D) {
			continue;
		}

		size_t numVerticesInMesh = pBindVertexBuffer->GetVertexCount();
		if (numVerticesInMesh == 0) {
			continue;
		}

		std::vector<DirectX::SimpleMath::Vector3> meshVertexPositions;
		meshVertexPositions.reserve(numVerticesInMesh);
		for (size_t i = 0; i < numVerticesInMesh; ++i)
		{
			Dvtx::ConstVertex constVertex = pBindVertexBuffer->GetVertex(i);
			if (hasPos3D) {
				meshVertexPositions.emplace_back(constVertex.Attr<Dvtx::VertexLayout::Position3D>());
			}
			else {
				const auto& pos2d = constVertex.Attr<Dvtx::VertexLayout::Position2D>();
				meshVertexPositions.emplace_back(pos2d.x, pos2d.y, 0.0f);
			}
		}

		UINT numIndices = pBindIndexBuffer->GetCount();
		if (numIndices == 0 || numIndices % 3 != 0) {
			continue;
		}

		for (UINT i = 0; i < numIndices; i += 3)
		{
			unsigned short i0 = pBindIndexBuffer->GetIndex(i);
			unsigned short i1 = pBindIndexBuffer->GetIndex(i + 1);
			unsigned short i2 = pBindIndexBuffer->GetIndex(i + 2);

			if (i0 >= meshVertexPositions.size() ||
				i1 >= meshVertexPositions.size() ||
				i2 >= meshVertexPositions.size())
			{
				continue;
			}

			allTriangles.emplace_back(
				meshVertexPositions[i0],
				meshVertexPositions[i1],
				meshVertexPositions[i2]
			);
		}
	}
	return allTriangles;
}

std::vector<DirectX::SimpleMath::Vector3> ModelComponent::GetAllUniqueVertices() const
{
	std::set<DirectX::XMFLOAT3, XMFLOAT3Less> uniqueVertexPositionsSet;

	for (const auto& pMeshUniquePtr : meshPtrs)
	{
		if (!pMeshUniquePtr) continue;
		const Mesh* pMesh = pMeshUniquePtr.get();

		std::shared_ptr<const Bind::VertexBuffer> pBindVertexBuffer = pMesh->GetVertexBuffer();
		if (!pBindVertexBuffer) continue;
		const Dvtx::VertexLayout& layout = pBindVertexBuffer->GetLayout();
		size_t numVertices = pBindVertexBuffer->GetVertexCount();

		bool hasPos3D = layout.Has(Dvtx::VertexLayout::Position3D);
		bool hasPos2D = layout.Has(Dvtx::VertexLayout::Position2D);

		if (!hasPos3D && !hasPos2D) {
			continue;
		}

		for (size_t i = 0; i < numVertices; ++i)
		{
			Dvtx::ConstVertex constVertex = pBindVertexBuffer->GetVertex(i);

			if (hasPos3D) {
				uniqueVertexPositionsSet.insert(constVertex.Attr<Dvtx::VertexLayout::Position3D>());
			}
			else if (hasPos2D) {
				const auto& pos2d = constVertex.Attr<Dvtx::VertexLayout::Position2D>();
				uniqueVertexPositionsSet.insert(DirectX::XMFLOAT3(pos2d.x, pos2d.y, 0.0f));
			}
		}
	}

	std::vector<DirectX::SimpleMath::Vector3> resultVertices;
	resultVertices.reserve(uniqueVertexPositionsSet.size());
	for (const auto& pos : uniqueVertexPositionsSet)
	{
		resultVertices.emplace_back(pos);
	}
	return resultVertices;
}

void ModelComponent::LinkTechniques(Rgph::RenderGraph& rg)
{
	for (auto& pMesh : meshPtrs)
	{
		pMesh->LinkTechniques(rg);
	}
}