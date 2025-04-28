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
// In ModelComponent.cpp

// --- UPDATED ParseMesh with Tangent/Bitangent and Normal Map Logic ---
// In ModelComponent.cpp

// --- UPDATED ParseMesh with Tangent/Bitangent and Normal Map Logic ---
std::unique_ptr<Mesh> ModelComponent::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::string& modelPath)
{
	using namespace Bind;
	using Dvtx::VertexLayout;
	namespace dx = DirectX;
	using namespace std::string_literals; // For "..."s string literals

	// --- Material Properties ---
	// These will be determined by loaded textures/material data
	bool hasDiffuseMap = false;
	bool hasSpecularMap = false;
	bool hasNormalMap = false;
	bool hasAlphaGloss = false; // Gloss stored in specular alpha channel?

	// Default material values, potentially overridden by Assimp material
	float shininess = 2.0f; // Default from example
	dx::XMFLOAT4 specularColor = { 0.18f, 0.18f, 0.18f, 1.0f }; // Default from example
	dx::XMFLOAT4 diffuseColor = { 0.45f, 0.45f, 0.85f, 1.0f };  // Default from example

	// This vector will hold the bindables for this specific mesh
	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	// --- Process Material Data (Textures, Colors, Shininess) ---
	if (mesh.mMaterialIndex >= 0) {
		const auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texPathAi;

		// Diffuse Texture (Slot 0)
		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texPathAi) == aiReturn_SUCCESS) {
			// Construct full path before resolving
			std::string diffusePath = modelPath + texPathAi.C_Str();
			bindablePtrs.push_back(Texture::Resolve(gfx, diffusePath, 0u));
			hasDiffuseMap = true;
		}
		else {
			// Load diffuse color if no map
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
			// diffuseColor.w = 1.0f; // Ensure alpha is 1 if loading from aiColor3D
		}

		// Specular Texture (Slot 1)
		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texPathAi) == aiReturn_SUCCESS) {
			std::string specPath = modelPath + texPathAi.C_Str();
			// Resolve the texture first to check for alpha
			auto pTex = Texture::Resolve(gfx, specPath, 1u);
			hasAlphaGloss = pTex->HasAlpha(); // Check if the loaded surface had non-255 alpha
			bindablePtrs.push_back(std::move(pTex)); // Add the resolved texture
			hasSpecularMap = true;
		}
		else {
			// Load specular color if no map
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
			// specularColor.w = 1.0f; // Ensure alpha is 1 if loading from aiColor3D
		}

		// Shininess / Specular Power (Load only if NOT using gloss map)
		if (!hasAlphaGloss) {
			material.Get(AI_MATKEY_SHININESS, shininess);
			// Clamp shininess to avoid issues with pow(0,0) or very large values if needed
			// shininess = std::max(shininess, 1.0f); 
		}

		// Normal Map Texture (Slot 2)
		if (material.GetTexture(aiTextureType_NORMALS, 0, &texPathAi) == aiReturn_SUCCESS) {
			std::string normPath = modelPath + texPathAi.C_Str();
			bindablePtrs.push_back(Texture::Resolve(gfx, normPath, 1u));
			hasNormalMap = true;
		}
		else if (material.GetTexture(aiTextureType_HEIGHT, 0, &texPathAi) == aiReturn_SUCCESS) {
			std::string normPath = modelPath + texPathAi.C_Str();
			bindablePtrs.push_back(Texture::Resolve(gfx, normPath, 1u));
			hasNormalMap = true;
		}

		// Sampler (Add if any texture was loaded)
		if (hasDiffuseMap || hasSpecularMap || hasNormalMap) {
			bindablePtrs.push_back(Sampler::Resolve(gfx));
		}
	}

	// --- Determine Vertex Layout based on available data (Normal Map requires Tangents) ---
	VertexLayout layout = VertexLayout{}
		.Append(VertexLayout::Position3D)
		.Append(VertexLayout::Normal);

	bool hasTangents = mesh.HasTangentsAndBitangents() && hasNormalMap; // Only need tangents if we HAVE a normal map
	if (hasTangents) {
		layout.Append(VertexLayout::Tangent)
			.Append(VertexLayout::Bitangent);
	}
	bool hasTexCoords = mesh.HasTextureCoords(0) && (hasDiffuseMap || hasSpecularMap || hasNormalMap); // Only need texcoords if a map uses them
	if (hasTexCoords) {
		layout.Append(VertexLayout::Texture2D);
	}


	// --- Create and Populate Vertex Buffer ---
	Dvtx::VertexBuffer vbuf(std::move(layout));
	const float scale = 1.0f; // Use scale from example if needed (e.g., 6.0f for gobber)
	// Or make it configurable
	for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
		DirectX::XMFLOAT3 pos = *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]);
		pos = { pos.x * scale, pos.y * scale, pos.z * scale }; // Apply scaling
		DirectX::XMFLOAT3 norm = *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]);
		DirectX::XMFLOAT3 tan = hasTangents ? *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]) : DirectX::XMFLOAT3{ 0,0,0 };
		DirectX::XMFLOAT3 bitan = hasTangents ? *reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]) : DirectX::XMFLOAT3{ 0,0,0 };
		DirectX::XMFLOAT2 tc = hasTexCoords ? *reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i]) : DirectX::XMFLOAT2{ 0,0 };

		// Emplace back based on the **actual final layout** determined above
		if (hasTangents && hasTexCoords) { // Layout: P, N, T, B, Tx
			vbuf.EmplaceBack(pos, norm, tan, bitan, tc);
		}
		else if (hasTangents && !hasTexCoords) { // Layout: P, N, T, B
			vbuf.EmplaceBack(pos, norm, tan, bitan);
		}
		else if (!hasTangents && hasTexCoords) { // Layout: P, N, Tx
			vbuf.EmplaceBack(pos, norm, tc);
		}
		else { // Layout: P, N
			vbuf.EmplaceBack(pos, norm);
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

	// --- Geometry Tags ---
	std::string geometryTag = modelPath + "%" + mesh.mName.C_Str();
	if (geometryTag.length() <= modelPath.length() + 1 && mesh.mName.length == 0) {
		geometryTag = modelPath + "%mesh" + std::to_string((int)mesh.mNumVertices);
	}

	// --- Resolve Geometry Bindables ---
	bindablePtrs.push_back(VertexBuffer::Resolve(gfx, geometryTag, vbuf));
	bindablePtrs.push_back(IndexBuffer::Resolve(gfx, geometryTag, indices));


	// --- Shader Selection & Pixel Constant Buffers ---
	std::shared_ptr<VertexShader> pvs;
	std::shared_ptr<PixelShader> pps;
	std::shared_ptr<InputLayout> pil;

	// Select shaders and setup pixel constant buffer based on material properties
	// Match the logic flow from the example Model::ParseMesh
	if (hasDiffuseMap && hasNormalMap && hasSpecularMap) {
		pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		pps = PixelShader::Resolve(gfx, "PhongPSSpecNormalMap.cso");

		// Define structure matching PhongPSSpecNormalMap.hlsl ObjectCBuf
		struct PSMaterialConstantFullmonte {
			BOOL  normalMapEnabled = TRUE;
			BOOL  specularMapEnabled = TRUE;
			BOOL  hasGlossMap = FALSE; // Will be set based on texture alpha
			float specularPower = 2.0f; // Default if hasGlossMap is false
			DirectX::XMFLOAT3 specularColor = { 0.18f,0.18f,0.18f }; // Default if hasGlossMap is true
			float specularMapWeight = 1.0f; // Default weight
		};
		PSMaterialConstantFullmonte pmc;
		pmc.specularPower = shininess; // Use loaded shininess if no gloss map
		pmc.hasGlossMap = hasAlphaGloss ? TRUE : FALSE;
		pmc.specularColor = { specularColor.x, specularColor.y, specularColor.z }; // Use loaded color if has gloss map
		// pmc.specularMapWeight = ...; // Can be adjusted if needed

		bindablePtrs.push_back(std::make_shared<PixelConstantBuffer<PSMaterialConstantFullmonte>>(gfx, pmc, 1u));

	}
	else if (hasDiffuseMap && hasNormalMap) {
		pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		pps = PixelShader::Resolve(gfx, "PhongPSNormalMap.cso");

		// Define structure matching PhongPSNormalMap.hlsl ObjectCBuf
		struct PSMaterialConstantDiffnorm {
			float specularIntensity = 0.18f; // Example default
			float specularPower;
			BOOL  normalMapEnabled = TRUE;
			float padding[1]; // Pad to 16 bytes (4 + 4 + 4 + 4)
		};
		PSMaterialConstantDiffnorm pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f; // Average specular color
		pmc.normalMapEnabled = TRUE;

		bindablePtrs.push_back(std::make_shared<PixelConstantBuffer<PSMaterialConstantDiffnorm>>(gfx, pmc, 1u));

	}
	else if (hasDiffuseMap) { // Implies no normal map, no specular map
		pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		pps = PixelShader::Resolve(gfx, "PhongPS.cso");
		// Define structure matching PhongPS.hlsl ObjectCBuf
		struct PSMaterialConstantDiffuse {
			float specularIntensity = 0.6f; // Example default
			float specularPower;
			float padding[2]; // Pad to 16 bytes (4 + 4 + 8)
		};
		PSMaterialConstantDiffuse pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

		bindablePtrs.push_back(std::make_shared<PixelConstantBuffer<PSMaterialConstantDiffuse>>(gfx, pmc, 1u));

	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap) {
		// Case for no textures at all
		pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
		pps = PixelShader::Resolve(gfx, "PhongPSNotex.cso");

		// Define structure matching PhongPSNotex.hlsl ObjectCBuf
		struct PSMaterialConstantNotex {
			DirectX::XMFLOAT4 materialColor; // Use loaded/default diffuse
			DirectX::XMFLOAT4 specularColor; // Use loaded/default specular
			float specularPower;
			float padding[3]; // Pad to 32 bytes (16 + 16 + 4 + 12)
		};
		PSMaterialConstantNotex pmc;
		pmc.specularPower = shininess;
		pmc.specularColor = specularColor;
		pmc.materialColor = diffuseColor;

		bindablePtrs.push_back(std::make_shared<PixelConstantBuffer<PSMaterialConstantNotex>>(gfx, pmc, 1u));
	}
	else {
		// Handle unsupported combinations
		throw std::runtime_error("Unsupported material texture combination for mesh: "s + mesh.mName.C_Str());
	}

	// Add the selected shaders
	auto pvsbc = pvs->GetBytecode(); // Get bytecode *after* resolving
	bindablePtrs.push_back(std::move(pvs));
	bindablePtrs.push_back(std::move(pps));

	// Resolve Input Layout based on final layout and VS
	bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));


	// Construct the Mesh object with the resolved bindables
	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
}

// --- ModelComponent::ShowWindow implementation remains the same ---
void ModelComponent::ShowWindow(const char* windowName) noexcept
{
	if (pRootInternal && pControlWindow)
	{
		pControlWindow->Show(windowName, *pRootInternal, pControlWindow->pSelectedNode, pControlWindow->transforms);
	}
}