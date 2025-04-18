#include "Node.h"
#include "ModelComponent.h" 
#include "Graphics.h"       
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include <cmath> // For math functions if needed elsewhere

namespace dx = DirectX;

Node::Node(std::string name, Node* parent)
    : name(std::move(name)), parent(parent)
{
    // Initialize stored components (already done via member initializers)
    // Initialize matrix caches
    dx::XMStoreFloat4x4(&localTransform, dx::XMMatrixIdentity());
    dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixIdentity());
    localTransformDirty = true; // Needs initial build
    worldTransformDirty = true; // Needs initial calculation
}

// --- Hierarchy and Component methods remain the same ---
void Node::AddChild(std::unique_ptr<Node> pChild) { /* ... same ... */
    assert(pChild);
    pChild->parent = this;
    children.push_back(std::move(pChild));
}
Node* Node::GetChild(size_t index) { /* ... same ... */
    if (index < children.size())
    {
        return children[index].get();
    }
    return nullptr;
}
Node* Node::GetParent() const { /* ... same ... */ return parent; }
const std::vector<std::unique_ptr<Node>>& Node::GetChildren() const { /* ... same ... */ return children; }
const std::string& Node::GetName() const { /* ... same ... */ return name; }

Component* Node::AddComponent(std::unique_ptr<Component> pComponent) { /* ... same ... */
    assert(pComponent);
    components.push_back(std::move(pComponent));
    return components.back().get();
}
const std::vector<std::unique_ptr<Component>>& Node::GetComponents() const { /* ... same ... */ return components; }


// --- Helper: Quaternion to Euler (needed ONLY for SetLocalTransform) ---
// Keep this function local to Node.cpp or in a separate utility file
DirectX::XMFLOAT3 QuaternionToEulerAnglesInternal(DirectX::XMFLOAT4 q)
{
    DirectX::XMFLOAT3 angles;
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.z = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp)); // Roll

    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.x = static_cast<float>(std::copysign(DirectX::XM_PI / 2, sinp));
    else
        angles.x = static_cast<float>(std::asin(sinp)); // Pitch

    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.y = static_cast<float>(std::atan2(siny_cosp, cosy_cosp)); // Yaw

    return { angles.x, angles.y, angles.z }; // Pitch, Yaw, Roll
}

// --- Helper: Build local matrix from stored components ---
void Node::UpdateLocalTransformFromComponents()
{
    if (localTransformDirty)
    {
        dx::XMMATRIX T = dx::XMMatrixScaling(localScale.x, localScale.y, localScale.z) *
            dx::XMMatrixRotationRollPitchYaw(localRotationEulerRad.x, localRotationEulerRad.y, localRotationEulerRad.z) *
            dx::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);
        dx::XMStoreFloat4x4(&localTransform, T);
        localTransformDirty = false; // Mark as clean
        worldTransformDirty = true;  // World transform depends on local, so mark it dirty
    }
}

// --- Helper: Update stored Euler angles when the whole matrix is set ---
void Node::UpdateStoredEulerFromMatrix()
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T);
    DirectX::XMFLOAT4 quat;
    dx::XMStoreFloat4(&quat, r);
    localRotationEulerRad = QuaternionToEulerAnglesInternal(quat); // Update stored Euler
    // Also update stored position and scale
    dx::XMStoreFloat3(&localPosition, t);
    dx::XMStoreFloat3(&localScale, s);
}

// --- UPDATED SetLocal... Methods ---

// This method now primarily exists if you need to set the transform directly via a matrix.
// It will decompose the matrix and update the stored pos/rot/scale components.
void Node::SetLocalTransform(DirectX::FXMMATRIX transform)
{
    dx::XMStoreFloat4x4(&localTransform, transform);
    UpdateStoredEulerFromMatrix(); // Update stored components from the new matrix
    localTransformDirty = false; // Matrix is explicitly set, so it's "clean"
    worldTransformDirty = true;  // World transform needs recalculation
}

void Node::SetLocalPosition(const DirectX::XMFLOAT3& pos)
{
    localPosition = pos;          // Update stored position
    localTransformDirty = true; // Mark matrix for rebuild
    worldTransformDirty = true; // Mark world matrix for rebuild
}

void Node::SetLocalRotation(const DirectX::XMFLOAT3& rotRad) // Input is Pitch, Yaw, Roll in Radians
{
    localRotationEulerRad = rotRad; // Update stored rotation
    localTransformDirty = true;   // Mark matrix for rebuild
    worldTransformDirty = true;   // Mark world matrix for rebuild
}

void Node::SetLocalScale(const DirectX::XMFLOAT3& scale)
{
    localScale = scale;           // Update stored scale
    localTransformDirty = true; // Mark matrix for rebuild
    worldTransformDirty = true; // Mark world matrix for rebuild
}


// --- UPDATED GetLocal... Methods ---

// Constructs the matrix from components if dirty
DirectX::XMMATRIX Node::GetLocalTransform() const
{
    if (localTransformDirty)
    {
        // Hacky const_cast or make localTransformDirty mutable
        const_cast<Node*>(this)->UpdateLocalTransformFromComponents();
    }
    return dx::XMLoadFloat4x4(&localTransform);
}

DirectX::XMFLOAT3 Node::GetLocalPosition() const
{
    return localPosition; // Return stored value
}

DirectX::XMFLOAT3 Node::GetLocalRotationEuler() const
{
    return localRotationEulerRad; // Return stored value
}

DirectX::XMFLOAT3 Node::GetLocalScale() const
{
    return localScale; // Return stored value
}

// --- World Transform Methods (GetWorldTransform, GetWorldPosition, SetWorldPosition) ---
// These rely on GetLocalTransform(), which now correctly rebuilds if needed.
// Their implementations remain the same as before.

DirectX::XMMATRIX Node::GetWorldTransform() const
{
    if (worldTransformDirty)
    {
        const_cast<Node*>(this)->UpdateWorldTransform();
    }
    return dx::XMLoadFloat4x4(&worldTransform);
}

DirectX::XMFLOAT3 Node::GetWorldPosition() const
{
    DirectX::XMMATRIX worldMat = GetWorldTransform();
    DirectX::XMFLOAT3 worldPos;
    dx::XMStoreFloat3(&worldPos, worldMat.r[3]);
    return worldPos;
}

void Node::SetWorldPosition(const DirectX::XMFLOAT3& worldPos)
{
    if (parent == nullptr)
    {
        SetLocalPosition(worldPos);
    }
    else
    {
        dx::XMMATRIX parentWorldTransform = parent->GetWorldTransform();
        dx::XMMATRIX invParentWorldTransform = dx::XMMatrixInverse(nullptr, parentWorldTransform);
        dx::XMVECTOR worldPosVec = dx::XMLoadFloat3(&worldPos);
        dx::XMVECTOR localPosVec = dx::XMVector3TransformCoord(worldPosVec, invParentWorldTransform);
        DirectX::XMFLOAT3 localPos;
        dx::XMStoreFloat3(&localPos, localPosVec);
        SetLocalPosition(localPos); // SetLocalPosition now updates stored pos and flags matrix dirty
    }
}

// --- UpdateWorldTransform - Depends on GetLocalTransform now ---
void Node::UpdateWorldTransform()
{
    // GetLocalTransform() will ensure the local matrix is up-to-date before proceeding
    dx::XMMATRIX local = GetLocalTransform();

    if (parent)
    {
        dx::XMMATRIX parentWorld = parent->GetWorldTransform(); // Ensures parent is up-to-date
        dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixMultiply(local, parentWorld));
    }
    else
    {
        dx::XMStoreFloat4x4(&worldTransform, local); // Root node's world is its local
    }
    worldTransformDirty = false; // Mark world as clean

    // Children's world transforms ARE now dirty because *this* node's world transform changed
    for (auto& child : children)
    {
        // Don't necessarily mark local dirty, just world
        child->worldTransformDirty = true;
    }
}

// --- Update - Update world first, then components/children ---
void Node::Update(float dt)
{
    // 1. Update this node's world transform (will rebuild local if needed)
    if (worldTransformDirty) {
        UpdateWorldTransform();
    }
    else if (localTransformDirty) {
        // If only local was dirty, world still needs update
        UpdateWorldTransform();
    }


    // 2. Update components
    for (auto& comp : components)
    {
        comp->Update(dt);
    }

    // 3. Update children recursively
    for (auto& child : children)
    {
        child->Update(dt);
    }
}

// --- Draw Method remains the same ---
void Node::Draw(Graphics& gfx) const
{
    if (auto* modelComp = GetComponent<ModelComponent>())
    {
        modelComp->Draw(gfx, GetWorldTransform());
    }
    for (const auto& child : children)
    {
        child->Draw(gfx);
    }
}

// --- ShowNodeTree remains the same ---
void Node::ShowNodeTree(Node*& pSelectedNode) noexcept
{
    // ... (Implementation remains the same) ...
    const intptr_t nodeId = reinterpret_cast<intptr_t>(this);
    const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
        ((pSelectedNode == this) ? ImGuiTreeNodeFlags_Selected : 0) |
        (children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

    const bool expanded = ImGui::TreeNodeEx(
        (void*)nodeId, node_flags, name.c_str()
    );

    if (ImGui::IsItemClicked())
    {
        pSelectedNode = this;
    }

    if (expanded)
    {
        for (const auto& pChild : children)
        {
            if (pChild)
            {
                pChild->ShowNodeTree(pSelectedNode);
            }
        }
        ImGui::TreePop();
    }
}