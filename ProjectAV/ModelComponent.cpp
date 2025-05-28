#include "ModelComponent.h"
#include "Mesh.h" 
#include "Node.h" 
#include "Graphics.h"
#include "Material.h" // Include the new Material class
#include "FrameCommander.h" // Include for Submit

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <scene.h>
#include "Bone.h"
#include "Vertex.h"
 
#include <DirectXMath.h>  // For DirectX::XMMATRIX, etc.
#include <algorithm> 

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
void ModelInternalNode::Submit(FrameCommander& frame, Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto modelNodeTransform =
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	for (const auto pm : meshPtrs) // pm is Mesh*
	{
		pm->Submit(frame, modelNodeTransform); // Call Mesh's Submit
	}
	for (const auto& pc : childPtrs)
	{
		pc->Submit(frame, gfx, modelNodeTransform); // Pass gfx if needed, or remove if not
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

ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale)
	: Component(owner, "ModelComponent"), // Corrected base Component constructor call
	pControlWindow(std::make_unique<ModelControlWindow>()) // Assuming ModelControlWindow still exists
{
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(modelFile.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights       // Important for skeletal animation
		//aiProcess_PopulateArmatureData    // Helpful for armature/bone info		
	);

	if (!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		// Use your ModelException or a std::runtime_error
		throw ModelException(__LINE__, __FILE__, "Assimp error: " + std::string(importer.GetErrorString()));
	}

	std::filesystem::path filePath(modelFile); // For material texture paths

	// 1. Parse materials from the scene (your existing logic for this)
	std::vector<Material> materials; // Assuming Material can be constructed and stored like this
	materials.reserve(pScene->mNumMaterials);
	for (size_t i = 0; i < pScene->mNumMaterials; ++i) {
		materials.emplace_back(gfx, *pScene->mMaterials[i], filePath); // Your Material constructor
	}

	// 2. Process each mesh in the scene
	meshPtrs.reserve(pScene->mNumMeshes); // meshPtrs is std::vector<std::unique_ptr<Mesh>>
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		const auto& assimpMesh = *pScene->mMeshes[i];

		//Material material(gfx, *pScene->mMaterials[assimpMesh.mMaterialIndex], &assimpMesh, filePath);


		const auto& material = materials[assimpMesh.mMaterialIndex]; // Get the material for this mesh

		// 2a. Determine the vertex layout required by this material for this mesh
		Dvtx::VertexLayout dvtxLayout = material.GetRequiredVertexLayout(assimpMesh);

		// 2b. Create the CPU-side Dvtx::VertexBuffer using this layout.
		// The Dvtx::VertexBuffer constructor (the one taking aiMesh) will populate
		// standard attributes (Position, Normal, TexCoord) based on its DVTX_ELEMENT_AI_EXTRACTOR logic.
		// Bone attributes will be default-initialized (e.g., to 0) by the fix in AttributeAiMeshFill.
		Dvtx::VertexBuffer dvtxBuffer(dvtxLayout, assimpMesh);

		// 2c. Apply scaling to vertex positions within the dvtxBuffer
		// (if the layout actually contains positions)
		if (dvtxLayout.Has(Dvtx::VertexLayout::Position3D)) {
			for (size_t vIdx = 0; vIdx < dvtxBuffer.Size(); ++vIdx) {
				auto& pos = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::Position3D>();
				pos.x *= scale;
				pos.y *= scale;
				pos.z *= scale;
			}
		}
		// Also scale normals if scale is non-uniform and you're not using inverse transpose world matrix.
		// For uniform scale, normals don't need rescaling here if they are just directions.

		// 2d. Extract bone weights and populate m_BoneInfoMap and bone data in dvtxBuffer
		// This happens ONLY if the layout determined by the material actually includes bone attributes.
		if (dvtxLayout.Has(Dvtx::VertexLayout::BoneIDs) && dvtxLayout.Has(Dvtx::VertexLayout::BoneWeights)) {
			if (assimpMesh.HasBones()) {
				// The mesh has bones, and the material wants bone data. Populate it.
				// The 'meshGlobalInverseTransform' argument is for advanced scenarios;
				// pass identity for now as aiBone::mOffsetMatrix is usually sufficient.
				ExtractBoneWeightForVertices(dvtxBuffer, assimpMesh, DirectX::XMMatrixIdentity());
			}
			// If layout Has BoneIDs/Weights but assimpMesh HasNObones, the default
			// initialization in AttributeAiMeshFill (e.g. boneID 0, weight 1.0) is used.
		}

		// 2e. Create the GPU-side Bind::VertexBuffer using the populated dvtxBuffer
		// ModelComponent calls Material::MakeVertexBindable to do this.
		// The 'scale' argument here can be 1.0f because we've already scaled dvtxBuffer.
		std::shared_ptr<Bind::VertexBuffer> pD3DVertexBuffer = material.MakeVertexBindable(
			gfx, assimpMesh, dvtxBuffer
		);

		// 2f. Create the GPU-side Bind::IndexBuffer
		std::shared_ptr<Bind::IndexBuffer> pD3DIndexBuffer = material.MakeIndexBindable(gfx, assimpMesh);

		// 2g. NOW, create the Mesh object using its NEW constructor
		meshPtrs.push_back(std::make_unique<Mesh>(
			this->pOwner,
			gfx,
			pD3DVertexBuffer,  // Pass the GPU vertex buffer
			pD3DIndexBuffer, // Pass the GPU index buffer
			material           // Pass the material (for techniques)
		));
	} // End of loop through meshes

	// 3. Parse the node hierarchy (your existing pRootInternal = ParseNodeRecursive(...))
	int nextId = 0;
	// The 'scale' passed to ParseNodeRecursive is for the transforms of ModelInternalNode,
	// not for individual vertex positions (which we've already scaled).
	pRootInternal = ParseNodeRecursive(nextId, *pScene->mRootNode, scale);

}

// **** CHANGED Draw to Submit ****
void ModelComponent::Submit(FrameCommander& frame, Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd
{
	if (pRootInternal) {
		pRootInternal->Submit(frame, gfx, worldTransform); // Pass gfx if ModelInternalNode::Submit needs it
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

 
void ModelComponent::ExtractBoneWeightForVertices(
	Dvtx::VertexBuffer& dvtxBuffer, // The CPU-side vertex buffer to fill
	const aiMesh& mesh,             // The Assimp mesh containing bone data
	const DirectX::XMMATRIX& meshGlobalInverseTransform // Currently unused, but good for future
) {
	// Ensure the layout of dvtxBuffer actually includes bone attributes.
	// If not, this function shouldn't have been called or there's a mismatch.
	if (!dvtxBuffer.GetLayout().Has(Dvtx::VertexLayout::BoneIDs) ||
		!dvtxBuffer.GetLayout().Has(Dvtx::VertexLayout::BoneWeights)) {
		// Output a warning or assert, as this indicates a logic error elsewhere.
		OutputDebugStringA("Warning: ExtractBoneWeightForVertices called for a mesh/material without bone attributes in its layout.\n");
		return;
	}

	// --- Step 1: Initialize Bone Data for all vertices in dvtxBuffer ---
	// The Dvtx::VertexBuffer constructor (via AttributeAiMeshFill fix) might have already
	// done a basic initialization. This ensures it's explicitly done or re-done here.
	for (size_t vIdx = 0; vIdx < dvtxBuffer.Size(); ++vIdx) {
		auto& bone_ids = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneIDs>();
		auto& bone_weights = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneWeights>();

		// Initialize all bone IDs to 0 (or a specific "no bone" ID if you have one)
		// and all weights to 0.0f.
		for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) {
			bone_ids[j] = 0;
			bone_weights[j] = 0.0f;
		}
	}

	// --- Step 2: Process each bone in the aiMesh ---
	for (unsigned int boneIdx = 0; boneIdx < mesh.mNumBones; ++boneIdx) {
		const aiBone* pAIBone = mesh.mBones[boneIdx];
		std::string boneName = pAIBone->mName.C_Str();
		int currentBoneAssignedID = -1; // This will be the ID used in the vertex attributes

		// Check if this bone is already in our map. If not, add it.
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter; // Assign the next available ID

			// Copy the offset matrix from Assimp.
			// aiMatrix4x4 is row-major. DirectX XMFLOAT4X4 can store it as is.
			// If your shaders/math expect column-major matrices from constant buffers,
			// you'll transpose EITHER here OR when sending to the AnimationComponent/shader.
			// Let's store it as Assimp provides it (row-major).
			// AnimationComponent will be responsible for any necessary transpose for the shader.
			memcpy(&newBoneInfo.offsetMatrix, &pAIBone->mOffsetMatrix, sizeof(DirectX::XMFLOAT4X4));
			// Example if you wanted to pre-transpose:
			// DirectX::XMMATRIX offset = DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&pAIBone->mOffsetMatrix));
			// DirectX::XMStoreFloat4x4(&newBoneInfo.offsetMatrix, DirectX::XMMatrixTranspose(offset));

			m_BoneInfoMap[boneName] = newBoneInfo;
			currentBoneAssignedID = m_BoneCounter;
			m_BoneCounter++; // Increment for the next unique bone

			// Optional: Warn if exceeding shader limits
			// if (m_BoneCounter > AnimationComponent::MAX_SHADER_BONES) { // Assuming AnimationComponent::MAX_SHADER_BONES is accessible
			//     OutputDebugStringA(("Warning: Bone count (" + std::to_string(m_BoneCounter) +
			//                         ") exceeds MAX_SHADER_BONES.\n").c_str());
			// }
		}
		else {
			// Bone already exists, get its ID
			currentBoneAssignedID = m_BoneInfoMap[boneName].id;
		}

		// --- Step 3: Assign weights for this bone to affected vertices ---
		for (unsigned int weightIdx = 0; weightIdx < pAIBone->mNumWeights; ++weightIdx) {
			const aiVertexWeight& vertexWeight = pAIBone->mWeights[weightIdx];
			unsigned int vertexId = vertexWeight.mVertexId;

			// Boundary check for vertexId
			if (vertexId >= dvtxBuffer.Size()) {
				OutputDebugStringA(("Warning: Vertex ID " + std::to_string(vertexId) +
					" from bone weight is out of bounds for dvtxBuffer size " +
					std::to_string(dvtxBuffer.Size()) + ".\n").c_str());
				continue; // Skip this weight
			}

			auto& bone_ids_for_vertex = dvtxBuffer[vertexId].Attr<Dvtx::VertexLayout::BoneIDs>();
			auto& bone_weights_for_vertex = dvtxBuffer[vertexId].Attr<Dvtx::VertexLayout::BoneWeights>();

			// Find an empty slot (weight == 0.0f) to add this bone's influence
			bool slotFound = false;
			for (int slot = 0; slot < MAX_BONES_PER_VERTEX; ++slot) {
				if (bone_weights_for_vertex[slot] == 0.0f) {
					bone_ids_for_vertex[slot] = currentBoneAssignedID;
					bone_weights_for_vertex[slot] = vertexWeight.mWeight;
					slotFound = true;
					break; // Move to the next vertexWeight for this bone
				}
			}

			// Optional: If no slot found (vertex is already influenced by MAX_BONES_PER_VERTEX)
			// You could implement a strategy to replace the smallest weight if the new weight is larger.
			// Assimp's aiProcess_LimitBoneWeights should ideally handle this before we get here,
			// ensuring each vertex has at most MAX_BONES_PER_VERTEX influences.
			if (!slotFound) {
				 OutputDebugStringA(("Warning: Vertex " + std::to_string(vertexId) +
				                     " has more than MAX_BONES_PER_VERTEX influences. Bone '" +
				                     boneName + "' weight not added.\n").c_str());
			}
		}
	} // End of loop through pAIBone in mesh

	// --- Step 4: Normalize bone weights for each vertex ---
	// Ensure that the sum of weights for each vertex equals 1.0.
	// aiProcess_LimitBoneWeights should also help with this.
	for (size_t vIdx = 0; vIdx < dvtxBuffer.Size(); ++vIdx) {
		auto& bone_ids_for_vertex = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneIDs>(); // To set default if no weights
		auto& bone_weights_for_vertex = dvtxBuffer[vIdx].Attr<Dvtx::VertexLayout::BoneWeights>();
		float totalWeight = 0.0f;

		for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) {
			totalWeight += bone_weights_for_vertex[j];
		}

		if (totalWeight > 0.00001f) { // Avoid division by zero or near-zero
			for (int j = 0; j < MAX_BONES_PER_VERTEX; ++j) {
				bone_weights_for_vertex[j] /= totalWeight;
			}
		}
		else {
			// If a vertex has no weights assigned from any bone (totalWeight is 0),
			// but the mesh itself HAS bones, it means this vertex is effectively static
			// or should follow a root bone.
			// Default it to be influenced 100% by bone ID 0 (assuming bone 0 is the root or a valid default).
			// This ensures it's still transformed if bone 0 is animated.
			if (mesh.mNumBones > 0 && MAX_BONES_PER_VERTEX > 0) { // Only if mesh is supposed to be skinned
				bone_ids_for_vertex[0] = 0; // Default to bone 0
				bone_weights_for_vertex[0] = 1.0f;
				for (int j = 1; j < MAX_BONES_PER_VERTEX; ++j) { // Zero out other slots
					bone_ids_for_vertex[j] = 0;
					bone_weights_for_vertex[j] = 0.0f;
				}
			}
			// If mesh.mNumBones == 0, the default init in AttributeAiMeshFill should have set weights[0]=1.0
		}
	}
}