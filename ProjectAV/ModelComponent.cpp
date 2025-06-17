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
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <set>
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace dx = DirectX;

struct TransformParameters
{
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};



class ModelControlWindow
{
public:

	void Show(const char* windowName, const ModelInternalNode& root, ModelInternalNode*& pSelectedNode,
		std::unordered_map<int, TransformParameters>& transforms) noexcept
	{
		windowName = windowName ? windowName : "Model Controls";
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);

			int nodeIndexTracker = 0;
			root.ShowTree(nodeIndexTracker, pSelectedNode);

			ImGui::NextColumn();
			if (pSelectedNode != nullptr)
			{
				const int selectedId = pSelectedNode->GetId();

				auto& transform = transforms[selectedId];


				ImGui::Text("Node: %s", pSelectedNode->GetName().c_str());
				ImGui::Separator();
				ImGui::Text("Orientation (Local Offset)");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position (Local Offset)");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);


			}
			else
			{
				ImGui::Text("Select a node in the tree.");
			}
		}
		ImGui::End();
	}


	ModelInternalNode* pSelectedNode = nullptr;

	std::unordered_map<int, TransformParameters> transforms;
};




ModelInternalNode::ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const dx::XMMATRIX& transform_in) noxnd
	: id(id), name(name), meshPtrs(std::move(meshPtrs))
{
	dx::XMStoreFloat4x4(&transform, transform_in);
	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity());
}

void ModelInternalNode::AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

void ModelInternalNode::Submit(Graphics& gfx, dx::FXMMATRIX accumulatedTransform, const std::vector<DirectX::XMMATRIX>* pBoneTransforms) const noxnd
{
	const auto modelNodeTransform =
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	// For each mesh in this node, submit it with the final transform AND the bone data
	for (const auto pm : meshPtrs)
	{
		// The Mesh::Submit overload will handle the pBoneTransforms pointer (even if null)
		pm->Submit(modelNodeTransform, pBoneTransforms);
	}

	// Recursively call submit for all children, passing the bone data down
	for (const auto& pc : childPtrs)
	{
		pc->Submit(gfx, modelNodeTransform, pBoneTransforms);
	}
}

int ModelInternalNode::GetId() const noexcept
{
	return id;
}

void ModelInternalNode::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&appliedTransform, transform);
}


void ModelInternalNode::ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode) const noexcept
{

	nodeIndexTracker++;

	const int currentId = GetId();
	const bool isSelected = (pSelectedNode != nullptr) && (pSelectedNode->GetId() == currentId);

	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		(isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
		(childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

	const bool expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)(currentId + nodeIndexTracker * 10000),
		node_flags, name.c_str()
	);

	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<ModelInternalNode*>(this);
	}

	if (expanded)
	{
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(nodeIndexTracker, pSelectedNode);
		}
		ImGui::TreePop();
	}
}

ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale, bool isSkinned)
	: Component(owner),
	pControlWindow(std::make_unique<ModelControlWindow>()),
	skinnedCharacter(isSkinned) // Initialize the member
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

	// Only extract bone info if the model is marked as skinned
	if (skinnedCharacter)
	{
		ExtractBoneInfo(*pScene);
	}

	std::filesystem::path filePath(modelFile);

	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);
	for (size_t i = 0; i < pScene->mNumMaterials; ++i) {
		// PASS THE FLAG to the material constructor
		materials.emplace_back(gfx, *pScene->mMaterials[i], filePath, skinnedCharacter);
	}

	meshPtrs.reserve(pScene->mNumMeshes);
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		const auto& assimpMesh = *pScene->mMeshes[i];
		meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[assimpMesh.mMaterialIndex], assimpMesh, scale));
	}

	int nextId = 0;
	pRootInternal = ParseNodeRecursive(nextId, *pScene->mRootNode, scale);
}

void ModelComponent::ExtractBoneInfo(const aiScene& scene)
{
	// Iterate through all meshes in the scene to find all unique bones
	for (unsigned int meshIdx = 0; meshIdx < scene.mNumMeshes; ++meshIdx)
	{
		const auto& mesh = *scene.mMeshes[meshIdx];
		if (!mesh.HasBones())
		{
			continue;
		}

		// Iterate through all bones in the current mesh
		for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx)
		{
			const auto& bone = *mesh.mBones[boneIdx];
			std::string boneName = bone.mName.C_Str();

			// If we haven't seen this bone before, add it to our map
			if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
			{
				BoneInfo newBoneInfo;
				newBoneInfo.id = m_BoneCounter;
				// Assimp matrix needs to be converted and transposed for DirectX.
				// This matrix transforms vertices from model space to the bone's local space.
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
	if (pRootInternal) {
		AnimationComponent* animationComponent = pOwner->GetComponent<AnimationComponent>();
		if (animationComponent != nullptr)
		{
			// Get the latest bone matrices from the animator
			const auto& boneMatrices = animationComponent->animator->GetFinalBoneMatrices();
			// Pass the world transform and the bone matrices into the recursive submit
			pRootInternal->Submit(gfx, worldTransform, &boneMatrices);
		}
		else
		{
			pRootInternal->Submit(gfx, worldTransform, nullptr);
		}
	}
}

std::unique_ptr<ModelInternalNode> ModelComponent::ParseNodeRecursive(int& nextId, const aiNode& node, float scale)
{
	namespace dx = DirectX;

	const auto transformMatrix = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	dx::XMMATRIX finalNodeTransform = transformMatrix;

	std::vector<Mesh*> currentNodeMeshPtrs;
	currentNodeMeshPtrs.reserve(node.mNumMeshes);
	for (unsigned int i = 0; i < node.mNumMeshes; ++i) {
		const auto meshIndex = node.mMeshes[i];
		if (meshIndex < meshPtrs.size()) {
			currentNodeMeshPtrs.push_back(meshPtrs[meshIndex].get());
		}
		else {
			OutputDebugStringA(("Warning: Mesh index out of bounds in node " + std::string(node.mName.C_Str()) + "\n").c_str());
		}
	}

	auto pNode = std::make_unique<ModelInternalNode>(nextId++, node.mName.C_Str(), std::move(currentNodeMeshPtrs), finalNodeTransform);

	for (unsigned int i = 0; i < node.mNumChildren; ++i) {
		pNode->AddChild(ParseNodeRecursive(nextId, *node.mChildren[i], scale));
	}
	return pNode;
}

void ModelComponent::ShowWindow(Graphics& gfx, const char* windowName) noexcept
{
	if (pRootInternal && pControlWindow) {
		pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
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