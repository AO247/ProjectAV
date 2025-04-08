#include "ModelComponent.h"
#include "Mesh.h" 
#include "Node.h" 
#include "Graphics.h"
#include "Surface.h"       // Still needed for Texture::Resolve internally, but not directly used here
#include "BindableCommon.h" // Include all necessary bindable headers
#include "Vertex.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "imgui/imgui.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>

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
void ModelInternalNode::Draw(Graphics& gfx, dx::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto modelNodeTransform =
		dx::XMLoadFloat4x4(&appliedTransform) *
		dx::XMLoadFloat4x4(&transform) *
		accumulatedTransform;

	// Draw meshes associated with this node
	for (const auto pm : meshPtrs)
	{
		// Set the transform on the mesh FIRST
		pm->SetTransform(modelNodeTransform);
		// THEN call the base Drawable::Draw which binds everything and issues DrawIndexed
		pm->Draw(gfx);
	}

	// Recursively draw children
	for (const auto& pc : childPtrs)
	{
		pc->Draw(gfx, modelNodeTransform);
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

ModelComponent::ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile)
	: Component(owner), pControlWindow(std::make_unique<ModelControlWindow>())
{
	// ... (Assimp loading remains the same) ...
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile(modelFile.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded | // Ensure left-handed coordinates
		aiProcess_GenNormals |          // Generate normals if missing
		aiProcess_CalcTangentSpace     // Optional: if using normal maps
	);

	if (!pScene || !pScene->mRootNode || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		throw ModelException(__LINE__, __FILE__, "Failed to load model file: " + modelFile + " | Assimp error: " + importer.GetErrorString());
	}

	// Extract path from model file for loading textures relative to it
	std::filesystem::path filePath(modelFile);
	std::string modelPath = filePath.parent_path().string();
	if (!modelPath.empty()) // Add trailing separator if path is not empty
	{
		modelPath += "\\";
	}


	// Parse materials and meshes first
	meshPtrs.reserve(pScene->mNumMeshes);
	for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
	{
		// Pass modelPath correctly
		meshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, modelPath));
	}

	// Parse node hierarchy recursively
	int nextId = 0; // Start ID counter for internal nodes
	pRootInternal = ParseNodeRecursive(nextId, *pScene->mRootNode);
}

ModelComponent::~ModelComponent() noexcept = default;

void ModelComponent::Draw(Graphics& gfx, dx::FXMMATRIX worldTransform) const noxnd
{
	if (pRootInternal)
	{
		pRootInternal->Draw(gfx, worldTransform);
	}
}

std::unique_ptr<ModelInternalNode> ModelComponent::ParseNodeRecursive(int& nextId, const aiNode& node) noexcept
{
	// ... (Implementation remains the same as before) ...
	namespace dx = DirectX;
	// Get the node's transform relative to its parent (and transpose for DirectX)
	const auto nodeTransform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

	// Gather pointers to the meshes this node uses
	std::vector<Mesh*> currentNodeMeshPtrs;
	currentNodeMeshPtrs.reserve(node.mNumMeshes);
	for (unsigned int i = 0; i < node.mNumMeshes; ++i)
	{
		const auto meshIndex = node.mMeshes[i];
		// Ensure meshIndex is valid before accessing meshPtrs
		if (meshIndex < meshPtrs.size())
		{
			currentNodeMeshPtrs.push_back(meshPtrs[meshIndex].get());
		}
		else {
			// Handle error or warning: mesh index out of bounds
			OutputDebugStringA(("Warning: Mesh index out of bounds in node " + std::string(node.mName.C_Str()) + "\n").c_str());
		}

	}

	// Create the internal node object
	auto pNode = std::make_unique<ModelInternalNode>(nextId++, node.mName.C_Str(), std::move(currentNodeMeshPtrs), nodeTransform);

	// Recursively parse children
	for (unsigned int i = 0; i < node.mNumChildren; ++i)
	{
		pNode->AddChild(ParseNodeRecursive(nextId, *node.mChildren[i]));
	}

	return pNode;
}


// --- UPDATED ParseMesh using BindableCodex::Resolve ---
std::unique_ptr<Mesh> ModelComponent::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::string& modelPath)
{
	using namespace Bind;
	using Dvtx::VertexLayout;
	namespace dx = DirectX;

	// --- Vertex Buffer Data ---
	VertexLayout layout = VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal);
	bool hasTexCoords = mesh.HasTextureCoords(0);
	if (hasTexCoords) { layout.Append(VertexLayout::Texture2D); }
	// Add Tangents etc. if needed
	Dvtx::VertexBuffer vbuf(std::move(layout));
	for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
		if (hasTexCoords) {
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}
		else {
			vbuf.EmplaceBack(
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}
	}

	// --- Index Buffer Data ---
	std::vector<unsigned short> indices;
	indices.reserve((size_t)mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; ++i) {
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	// This vector will hold the bindables for this specific mesh
	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	// Generate a unique tag for geometry buffers based on model path and mesh name
	std::string geometryTag = modelPath + "%" + mesh.mName.C_Str();
	// Ensure tag is not empty or just "%"
	if (geometryTag.length() <= 1 && mesh.mName.length == 0) {
		geometryTag = modelPath + "%mesh" + std::to_string(mesh.mNumVertices); // Fallback tag
	}

	// --- Resolve / Create Bindables using Codex ---

	// Vertex Buffer
	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, geometryTag, vbuf));

	// Index Buffer
	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, geometryTag, indices));

	// Vertex Shader
	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso"); // Use std::string path
	auto pvsbc = pvs->GetBytecode(); // Still need bytecode for Input Layout
	bindablePtrs.push_back(std::move(pvs));

	// Input Layout
	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

	// --- Material Properties (Textures, Shaders, Constants) ---
	bool hasSpecularMap = false;
	float shininess = 40.0f;
	aiColor3D diffuseColor(1.0f, 1.0f, 1.0f);
	aiColor3D specularColor(1.0f, 1.0f, 1.0f);

	if (mesh.mMaterialIndex >= 0) {
		const auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texPathAi; // Use aiString for Assimp calls

		// Diffuse Texture
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texPathAi) == aiReturn_SUCCESS) {
			bindablePtrs.push_back(Texture::Resolve(gfx, modelPath + texPathAi.C_Str(), 0u)); // Slot 0
		}
		else {
			material.Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
			// Maybe add a default white texture here if desired?
			// bindablePtrs.push_back(Texture::Resolve(gfx, "path/to/defaultwhite.png", 0u)); 
		}

		// Specular Texture
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texPathAi) == aiReturn_SUCCESS) {
			bindablePtrs.push_back(Texture::Resolve(gfx, modelPath + texPathAi.C_Str(), 1u)); // Slot 1
			hasSpecularMap = true;
		}
		else {
			material.Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		// Sampler (Common for most textured meshes)
		bindablePtrs.push_back(Sampler::Resolve(gfx));

		// Pixel Shader (Choose based on specular map presence)
		if (hasSpecularMap) {
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpecMap.cso")); // Use std::string path
		}
		else {
			bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso")); // Use std::string path

			// Pixel Constant Buffer for Material (No Specular Map)
			struct PSMaterialConstant {
				dx::XMFLOAT3 specularColor = { 1.0f, 1.0f, 1.0f }; // 12 bytes
				float specularPower;                               //  4 bytes
				// Total size = 16 bytes
			} pmc;
			pmc.specularColor = { specularColor.r, specularColor.g, specularColor.b };
			pmc.specularPower = shininess > 0.0f ? shininess : 1.0f;

			// *** Potential Issue: Resolve might share this buffer incorrectly ***
			// If all meshes get the same shininess, change Resolve to make_shared:
			bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
			// bindablePtrs.push_back(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u)); // Alternative if Resolve causes issues
		}
		// Add Normal Map Texture if present & shader supports it
		// if (material.GetTexture(aiTextureType_NORMALS, 0, &texPathAi) == aiReturn_SUCCESS) { 
		//     bindablePtrs.push_back(Texture::Resolve(gfx, modelPath + texPathAi.C_Str(), 2u)); // Slot 2 for normal map
		// }
	}
	else // Default material if mesh.mMaterialIndex < 0
	{
		bindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
		struct PSMaterialConstant {
			dx::XMFLOAT3 specularColor = { 1.0f, 1.0f, 1.0f }; // 12 bytes
			float specularPower = 40.0f;                      //  4 bytes
		} pmc;
		// Use Resolve (or make_shared if needed)
		bindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, pmc, 1u));
		bindablePtrs.push_back(Sampler::Resolve(gfx));
	}

	// Construct the Mesh object with the resolved bindables
	// The Mesh constructor now takes the vector of shared_ptrs
	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}


void ModelComponent::ShowWindow(const char* windowName) noexcept
{
	// ... (Implementation remains the same as before) ...
	if (pRootInternal && pControlWindow)
	{
		// The types passed here should now match the corrected Show signature
		pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
	}
}