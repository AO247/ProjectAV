#include "Node.h"
#include "ModelComponent.h" // Include components that have Draw methods
#include "Graphics.h"       // Include Graphics for the Draw method
#include <DirectXMath.h>
#include "imgui/imgui.h"
namespace dx = DirectX;

Node::Node(std::string name, Node* parent)
    : name(std::move(name)), parent(parent)
{
    dx::XMStoreFloat4x4(&localTransform, dx::XMMatrixIdentity());
    dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixIdentity());
    worldTransformDirty = true; // Initially needs calculation
}

void Node::AddChild(std::unique_ptr<Node> pChild)
{
    assert(pChild);
    pChild->parent = this; // Set parent pointer
    children.push_back(std::move(pChild));
}

Node* Node::GetChild(size_t index)
{
    if (index < children.size())
    {
        return children[index].get();
    }
    return nullptr;
}

Node* Node::GetParent() const
{
    return parent;
}

const std::vector<std::unique_ptr<Node>>& Node::GetChildren() const
{
    return children;
}

const std::string& Node::GetName() const
{
    return name;
}

Component* Node::AddComponent(std::shared_ptr<Component> pComponent)
{
    assert(pComponent);
    components.push_back(pComponent);
    return components.back().get(); // Return raw pointer to added component
}

void Node::SetLocalTransform(DirectX::FXMMATRIX transform)
{
    dx::XMStoreFloat4x4(&localTransform, transform);
    worldTransformDirty = true;
}

void Node::SetLocalPosition(const DirectX::XMFLOAT3& pos)
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    // Extract rotation and scale
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T);
    // Create new matrix with new translation
    T = dx::XMMatrixScalingFromVector(s) * dx::XMMatrixRotationQuaternion(r) * dx::XMMatrixTranslation(pos.x, pos.y, pos.z);
    SetLocalTransform(T);
}

void Node::SetLocalRotation(const DirectX::XMFLOAT3& rotRad) // rotRad is {Pitch, Yaw, Roll} in radians
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T);
    // --- Use individual components for RollPitchYaw ---
    // Assuming rotRad.x = Pitch, rotRad.y = Yaw, rotRad.z = Roll
    T = dx::XMMatrixScalingFromVector(s) *
        dx::XMMatrixRotationRollPitchYaw(rotRad.x, rotRad.y, rotRad.z) * // Pass floats
        dx::XMMatrixTranslationFromVector(t);
    SetLocalTransform(T);
}

void Node::SetLocalScale(const DirectX::XMFLOAT3& scale)
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    // Extract rotation and translation
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T);
    // Create new matrix with new scale
    T = dx::XMMatrixScaling(scale.x, scale.y, scale.z) * dx::XMMatrixRotationQuaternion(r) * dx::XMMatrixTranslationFromVector(t);
    SetLocalTransform(T);
}


DirectX::XMMATRIX Node::GetLocalTransform() const
{
    return dx::XMLoadFloat4x4(&localTransform);
}

DirectX::XMMATRIX Node::GetWorldTransform() const
{
    // If dirty, recalculate. Use const_cast because calculation modifies mutable state (cache)
    // Or make worldTransformDirty mutable
    if (worldTransformDirty)
    {
        // This is slightly hacky with const. Making worldTransformDirty mutable is cleaner.
        const_cast<Node*>(this)->UpdateWorldTransform();
    }
    return dx::XMLoadFloat4x4(&worldTransform);
}

DirectX::XMFLOAT3 QuaternionToEulerAngles(DirectX::XMFLOAT4 q)
{
    DirectX::XMFLOAT3 angles;

    // Roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.z = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp)); // Roll

    // Pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.x = static_cast<float>(std::copysign(DirectX::XM_PI / 2, sinp)); // Use 90 degrees if out of range
    else
        angles.x = static_cast<float>(std::asin(sinp)); // Pitch

    // Yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.y = static_cast<float>(std::atan2(siny_cosp, cosy_cosp)); // Yaw

    return { angles.x, angles.y, angles.z }; // Return Pitch, Yaw, Roll order directly
}


DirectX::XMFLOAT3 Node::GetLocalPosition() const
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T);
    DirectX::XMFLOAT3 pos;
    dx::XMStoreFloat3(&pos, t);
    return pos;
}

DirectX::XMFLOAT3 Node::GetLocalRotationEuler() const
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T); // r is the quaternion
    DirectX::XMFLOAT4 quat;
    dx::XMStoreFloat4(&quat, r);
    return QuaternionToEulerAngles(quat); // Convert quaternion to Euler (radians)
}

DirectX::XMFLOAT3 Node::GetLocalScale() const
{
    dx::XMMATRIX T = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s, r, t;
    dx::XMMatrixDecompose(&s, &r, &t, T);
    DirectX::XMFLOAT3 scale;
    dx::XMStoreFloat3(&scale, s);
    return scale;
}

const std::vector<std::shared_ptr<Component>>& Node::GetComponents() const
{
    return components;
}

void Node::UpdateWorldTransform()
{
    if (worldTransformDirty)
    {
        dx::XMMATRIX local = dx::XMLoadFloat4x4(&localTransform);
        if (parent)
        {
            dx::XMMATRIX parentWorld = parent->GetWorldTransform(); // Recursive call ensures parent is up-to-date
            dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixMultiply(local, parentWorld));
        }
        else
        {
            dx::XMStoreFloat4x4(&worldTransform, local); // Root node
        }
        worldTransformDirty = false; // Mark as clean

        // Children's world transforms are now dirty
        for (auto& child : children)
        {
            child->worldTransformDirty = true;
        }
    }
}




void Node::Update(float dt)
{
    // 1. Update this node's world transform if needed (relative to parent)
    UpdateWorldTransform();

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

void Node::Draw(Graphics& gfx) const
{
    // 1. Draw components attached to this node that are drawable
    //    (We only have ModelComponent for now)
    if (auto modelComp = GetComponent<ModelComponent>()) // Checks if a ModelComponent exists
    {
        modelComp->Draw(gfx, GetWorldTransform()); // Pass the final world transform
    }

    // 2. Draw children recursively
    for (const auto& child : children)
    {
        child->Draw(gfx);
    }
}
void Node::ShowNodeTree(Node*& pSelectedNode) noexcept
{
    // Use the node's pointer as a unique ID for ImGui
    const intptr_t nodeId = reinterpret_cast<intptr_t>(this);

    // Determine node flags
    const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
        ((pSelectedNode == this) ? ImGuiTreeNodeFlags_Selected : 0) |
        (children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

    // Render this node
    const bool expanded = ImGui::TreeNodeEx(
        (void*)nodeId, // Unique ID for ImGui
        node_flags,
        name.c_str()    // Display node name
    );

    // Handle selection
    if (ImGui::IsItemClicked())
    {
        pSelectedNode = this; // Update the selected node pointer passed by reference
    }

    // Recurse for children if the node is expanded
    if (expanded)
    {
        for (const auto& pChild : children)
        {
            if (pChild) // Basic safety check
            {
                pChild->ShowNodeTree(pSelectedNode); // Pass the reference down
            }
        }
        ImGui::TreePop();
    }
}