#include "ModelComponent.h"
#include "Mesh.h" 
#include "Node.h" 
#include "Graphics.h"
#include "Material.h" // Include the new Material class
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

// --- TransformParameters, ModelControlWindow, ModelInternalNode implementations remain the same ---

// ... (Struct TransformParameters, class ModelControlWindow, class ModelInternalNode implementations) ...
// --- Define TransformParameters OUTSIDE and BEFORE ModelControlWindow ---
// Helper struct for transform params within the map
struct TransformParameters
{
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};


// --- PImpl Class for ImGui Window ---
// Put this definition inside the .cpp file
class ModelControlWindow
{
public:
	// --- Correct the signature for the 'transforms' map parameter ---
	void Show(const char* windowName, const ModelInternalNode& root, ModelInternalNode*& pSelectedNode,
		std::unordered_map<int, TransformParameters>& transforms) noexcept // Corrected type
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
				// --- This line should now work correctly ---
				auto& transform = transforms[selectedId];

				// --- Use GetName() method (see fix #3) ---
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

				// Apply the transform to the node
				// --- Fix Rotation Order (Pitch, Yaw, Roll for XMMatrixRotationRollPitchYaw) ---
				// --- REMOVED THIS based on previous step, uncomment if needed ---
				/*
				pSelectedNode->SetAppliedTransform(
					dx::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) * // Correct order
					dx::XMMatrixTranslation(transform.x, transform.y, transform.z)
				);
				*/
			}
			else
			{
				ImGui::Text("Select a node in the tree.");
			}
		}
		ImGui::End();
	}

	// Store selected node and transforms here, managed by ModelComponent instance
	ModelInternalNode* pSelectedNode = nullptr;
	// --- Ensure map type uses the now globally defined TransformParameters ---
	std::unordered_map<int, TransformParameters> transforms;
};


// --- ModelInternalNode Implementation --- (Mostly from original Node inside Model)

ModelInternalNode::ModelInternalNode(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const dx::XMMATRIX& transform_in) noxnd
	: id(id), name(name), meshPtrs(std::move(meshPtrs))
{
	dx::XMStoreFloat4x4(&transform, transform_in); // Store the transform from the file
	dx::XMStoreFloat4x4(&appliedTransform, dx::XMMatrixIdentity()); // Init applied transform
}

void ModelInternalNode::AddChild(std::unique_ptr<ModelInternalNode> pChild) noxnd
{
	assert(pChild);
	childPtrs.push_back(std::move(pChild));
}

// Draw method for the *internal* node structure
void ModelInternalNode::Submit(Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto modelNodeTransform =
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs) // pm is Mesh*
	{
		pm->Submit(modelNodeTransform); // Call Mesh's Submit
	}
	for (const auto& pc : childPtrs)
	{
		pc->Submit(gfx, modelNodeTransform); // Pass gfx if needed, or remove if not
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

// ShowTree function for ImGui - adapted to use the pSelectedNode reference
void ModelInternalNode::ShowTree(int& nodeIndexTracker, ModelInternalNode*& pSelectedNode) const noexcept
{
	// ... (Implementation remains the same as before) ...
	nodeIndexTracker++; // Increment tracker for unique ID

	// Check if this node is the selected one
	const int currentId = GetId();
	const bool isSelected = (pSelectedNode != nullptr) && (pSelectedNode->GetId() == currentId);

	// Node flags for ImGui
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		(isSelected ? ImGuiTreeNodeFlags_Selected : 0) |
		(childPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

	// Use node ID and tracker for a unique ImGui ID
	const bool expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)(currentId + nodeIndexTracker * 10000), // Combine ID with tracker for uniqueness
		node_flags, name.c_str()
	);

	// Handle selection click
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<ModelInternalNode*>(this); // Update the selected node reference
	}

	// Recurse for children if expanded
	if (expanded)
	{
		for (const auto& pChild : childPtrs)
		{
			pChild->ShowTree(nodeIndexTracker, pSelectedNode); // Pass reference down
		}
		ImGui::TreePop();
	}
}


// --- ModelComponent Implementation ---

ModelComponent::ModelComponent(Node* owner, Graphics& gfx, Rgph::RenderGraph& rg, const std::string& modelFile, float scale)
	: Component(owner), pControlWindow(std::make_unique<ModelControlWindow>())
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

	std::filesystem::path filePath(modelFile); // Used by Material class

	// Parse materials FIRST
	std::vector<Material> materials; // Local vector to hold parsed materials
	materials.reserve(pScene->mNumMaterials);
	for (size_t i = 0; i < pScene->mNumMaterials; ++i) {
		// Material constructor takes path now
		materials.emplace_back(gfx, *pScene->mMaterials[i], filePath);
	}

	// Parse meshes, creating Mesh objects using the parsed Materials
	meshPtrs.reserve(pScene->mNumMeshes);
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i) {
		const auto& assimpMesh = *pScene->mMeshes[i];
		// Mesh constructor now takes Graphics, Material, aiMesh, scale
		meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[assimpMesh.mMaterialIndex], assimpMesh, scale));
	}

	int nextId = 0;
	pRootInternal = ParseNodeRecursive(nextId, *pScene->mRootNode, scale);
	LinkTechniques(rg);
}


// **** CHANGED Draw to Submit ****
void ModelComponent::Submit(Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd
{
	if (pRootInternal) {
		pRootInternal->Submit(gfx, worldTransform); // Pass gfx if ModelInternalNode::Submit needs it
	}
}

// ParseMesh function is removed from ModelComponent, as Material and Mesh constructors handle it.

std::unique_ptr<ModelInternalNode> ModelComponent::ParseNodeRecursive(int& nextId, const aiNode& node, float scale)
{
	namespace dx = DirectX;
	// Use ScaleTranslation from example if it exists, otherwise regular matrix math
	const auto transformMatrix = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));
	// The example Model::ParseNode applies scaling to the node transform here.
	// If your Mesh takes a scale parameter, this might be redundant or need adjustment.
	// Let's assume ScaleTranslation is available and correct.
	// If not, you can decompose, scale translation, and recompose, or apply scale in Mesh.
	dx::XMMATRIX finalNodeTransform = transformMatrix; // Modify if using ScaleTranslation
	// If ScaleTranslation is a global helper:
	// dx::XMMATRIX finalNodeTransform = ScaleTranslation(transformMatrix, scale);


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
		// pControlWindow->Show(gfx, windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
		// The ImGui window now needs different parameters or different logic
		// The example ModelWindow has its own Material instances and ControlMeDaddy.
		// You'll need to adapt this or create a new ImGui window for your ModelComponent
		// that perhaps allows selecting techniques or modifying common material parameters
		// exposed via a TechniqueProbe.
		// For now, let's just make it a simple transform editor for the root ModelInternalNode:
		pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
	}
}

std::vector<ModelComponent::Triangle> ModelComponent::GetAllTriangles() const
{
	std::vector<ModelComponent::Triangle> allTriangles;

	if (meshPtrs.empty()) {
		return allTriangles; // Early exit if no meshes
	}

	for (const auto& pMeshUniquePtr : meshPtrs)
	{
		if (!pMeshUniquePtr) {
			continue; // Skip null mesh pointers
		}
		const Mesh* pMesh = pMeshUniquePtr.get();

		std::shared_ptr<const Bind::VertexBuffer> pBindVertexBuffer = pMesh->GetVertexBuffer();
		std::shared_ptr<const Bind::IndexBuffer> pBindIndexBuffer = pMesh->GetIndexBuffer();

		if (!pBindVertexBuffer || !pBindIndexBuffer) {
			// Mesh is missing necessary buffers, skip it
			continue;
		}

		const Dvtx::VertexLayout& layout = pBindVertexBuffer->GetLayout();
		bool hasPos3D = layout.Has(Dvtx::VertexLayout::Position3D);
		bool hasPos2D = layout.Has(Dvtx::VertexLayout::Position2D);

		if (!hasPos3D && !hasPos2D) {
			// Mesh vertices have no position data, skip it
			continue;
		}

		size_t numVerticesInMesh = pBindVertexBuffer->GetVertexCount();
		if (numVerticesInMesh == 0) {
			// No vertices in this mesh, skip it
			continue;
		}

		// Pre-fetch all vertex positions for this mesh for efficient lookup
		std::vector<DirectX::SimpleMath::Vector3> meshVertexPositions;
		meshVertexPositions.reserve(numVerticesInMesh);
		for (size_t i = 0; i < numVerticesInMesh; ++i)
		{
			Dvtx::ConstVertex constVertex = pBindVertexBuffer->GetVertex(i);
			if (hasPos3D) {
				meshVertexPositions.emplace_back(constVertex.Attr<Dvtx::VertexLayout::Position3D>());
			}
			else { // hasPos2D must be true if we reached here
				const auto& pos2d = constVertex.Attr<Dvtx::VertexLayout::Position2D>();
				meshVertexPositions.emplace_back(pos2d.x, pos2d.y, 0.0f);
			}
		}

		// Process indices to form triangles
		UINT numIndices = pBindIndexBuffer->GetCount();
		if (numIndices == 0 || numIndices % 3 != 0) {
			// Invalid number of indices for triangles, or no indices; skip this mesh's triangles
			continue;
		}

		for (UINT i = 0; i < numIndices; i += 3)
		{
			unsigned short i0 = pBindIndexBuffer->GetIndex(i);
			unsigned short i1 = pBindIndexBuffer->GetIndex(i + 1);
			unsigned short i2 = pBindIndexBuffer->GetIndex(i + 2);

			// Bounds check for indices against the fetched vertex positions for this specific mesh
			if (i0 >= meshVertexPositions.size() ||
				i1 >= meshVertexPositions.size() ||
				i2 >= meshVertexPositions.size())
			{
				// Index out of bounds for this mesh's vertex data. This shouldn't happen
				// if the model data is valid. Log an error or assert in a debug build if desired.
				// For release, skipping the problematic triangle is a reasonable recovery.
				// Consider: assert(false && "Triangle index out of bounds!");
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
	// Remove or comment out: using namespace DXCompare;

	// **** USE THE CUSTOM COMPARATOR HERE ****
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