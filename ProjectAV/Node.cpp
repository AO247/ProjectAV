#include "Node.h"
#include "ModelComponent.h" 
#include "Graphics.h"       
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include <cmath> // For std::atan2, std::asin, std::copysign

// Forward declare FrameCommander if it's only used as a reference/pointer type in Submit
// class FrameCommander; 
// No, Submit needs its definition if methods are called on it. Assume it's included elsewhere or defined.

namespace dx = DirectX;

Node::Node(std::string name, Node* parent, std::string tag)
    : name(std::move(name)), parent(parent), tag(std::move(tag)),
    localPosition{ 0.0f, 0.0f, 0.0f },
    localRotationQuaternion{ 0.0f, 0.0f, 0.0f, 1.0f }, // Identity quaternion
    localScale{ 1.0f, 1.0f, 1.0f }
{
    dx::XMStoreFloat4x4(&localTransform, dx::XMMatrixIdentity());
    dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixIdentity());
    localTransformDirty = true;
    worldTransformDirty = true;
}

// --- Hierarchy and Component methods (unchanged from original provided snippet) ---
void Node::AddChild(std::unique_ptr<Node> pChild) {
    assert(pChild);
    pChild->parent = this;
    children.push_back(std::move(pChild));
}
Node* Node::GetChild(size_t index) {
    if (index < children.size())
    {
        return children[index].get();
    }
    return nullptr;
}
Node* Node::GetParent() const { return parent; }
const std::vector<std::unique_ptr<Node>>& Node::GetChildren() const { return children; }
const std::string& Node::GetName() const { return name; }

Component* Node::AddComponent(std::unique_ptr<Component> pComponent) {
    assert(pComponent);
    components.push_back(std::move(pComponent));
    return components.back().get();
}
const std::vector<std::unique_ptr<Component>>& Node::GetComponents() const { return components; }

// --- Helper: Quaternion to Euler (Pitch, Yaw, Roll) ---
// Returns XMFLOAT3 {Pitch, Yaw, Roll} in radians
DirectX::XMFLOAT3 QuaternionToEulerAnglesInternal(DirectX::XMFLOAT4 q)
{
    DirectX::XMFLOAT3 angles; // Pitch, Yaw, Roll

    // Roll (z-axis rotation)
    double sinr_cosp = 2.0 * (q.w * q.z + q.x * q.y);
    double cosr_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    angles.z = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp));

    // Pitch (x-axis rotation)
    double sinp = 2.0 * (q.w * q.x - q.y * q.z);
    if (std::abs(sinp) >= 1.0)
        angles.x = static_cast<float>(std::copysign(DirectX::XM_PI / 2.0, sinp)); // Use 90 degrees if out of range
    else
        angles.x = static_cast<float>(std::asin(sinp));

    // Yaw (y-axis rotation)
    double siny_cosp = 2.0 * (q.w * q.y + q.z * q.x);
    double cosy_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    angles.y = static_cast<float>(std::atan2(siny_cosp, cosy_cosp));

    return { angles.x, angles.y, angles.z }; // {Pitch, Yaw, Roll}
}

// --- Helper: Build local matrix from stored pos/quat/scale ---
void Node::UpdateLocalTransformFromComponents()
{
    if (localTransformDirty)
    {
        dx::XMMATRIX matS = dx::XMMatrixScaling(localScale.x, localScale.y, localScale.z);
        dx::XMMATRIX matR = dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&localRotationQuaternion));
        dx::XMMATRIX matT = dx::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);

        // Original order: Scale * Rotate * Translate
        // This means operations applied to a vector v are: T*v, then R*(T*v), then S*(R*(T*v))
        // So, Translation first, then Rotation, then Scaling.
        dx::XMStoreFloat4x4(&localTransform, matS * matR * matT);

        localTransformDirty = false;
        worldTransformDirty = true;
    }
}

// --- Helper: Update stored pos/quat/scale when the whole matrix is set ---
void Node::UpdateStoredComponentsFromMatrix()
{
    dx::XMMATRIX matrix = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s_vec, r_quat_vec, t_vec;
    dx::XMMatrixDecompose(&s_vec, &r_quat_vec, &t_vec, matrix);

    dx::XMStoreFloat3(&localPosition, t_vec);
    dx::XMStoreFloat4(&localRotationQuaternion, r_quat_vec);
    dx::XMStoreFloat3(&localScale, s_vec);
}

// --- SetLocal... Methods ---

void Node::SetLocalTransform(DirectX::FXMMATRIX transform)
{
    dx::XMStoreFloat4x4(&localTransform, transform);
    UpdateStoredComponentsFromMatrix(); // Update stored pos, quat, scale from the new matrix
    localTransformDirty = false;
    worldTransformDirty = true;
}

void Node::SetLocalPosition(const DirectX::XMFLOAT3& pos)
{
    localPosition = pos;
    localTransformDirty = true;
    worldTransformDirty = true;
}

// rotRad is Pitch, Yaw, Roll in Radians
void Node::SetLocalRotation(const DirectX::XMFLOAT3& rotRad)
{
    dx::XMVECTOR q = dx::XMQuaternionRotationRollPitchYaw(rotRad.x, rotRad.y, rotRad.z);
    dx::XMStoreFloat4(&localRotationQuaternion, q);
    localTransformDirty = true;
    worldTransformDirty = true;
}

void Node::SetLocalRotation(const DirectX::XMFLOAT4& quat)
{
    localRotationQuaternion = quat;
    // It's good practice to normalize the quaternion if it's coming from an external source
    // dx::XMStoreFloat4(&localRotationQuaternion, dx::XMQuaternionNormalize(dx::XMLoadFloat4(&quat)));
    localTransformDirty = true;
    worldTransformDirty = true;
}

void Node::SetLocalScale(const DirectX::XMFLOAT3& scale)
{
    localScale = scale;
    localTransformDirty = true;
    worldTransformDirty = true;
}

void Node::TranslateLocal(const DirectX::XMFLOAT3& translation)
{
    dx::XMMATRIX rotMatrix = dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&localRotationQuaternion));
    dx::XMVECTOR translationVec = dx::XMLoadFloat3(&translation);
    dx::XMVECTOR rotatedTranslationVec = dx::XMVector3TransformNormal(translationVec, rotMatrix);

    dx::XMVECTOR currentPosVec = dx::XMLoadFloat3(&localPosition);
    dx::XMVECTOR newPosVec = dx::XMVectorAdd(currentPosVec, rotatedTranslationVec);
    dx::XMStoreFloat3(&localPosition, newPosVec);

    localTransformDirty = true;
    worldTransformDirty = true;
}

// --- GetLocal... Methods ---

DirectX::XMMATRIX Node::GetLocalTransform() const
{
    if (localTransformDirty)
    {
        const_cast<Node*>(this)->UpdateLocalTransformFromComponents();
    }
    return dx::XMLoadFloat4x4(&localTransform);
}

DirectX::XMFLOAT3 Node::GetLocalPosition() const
{
    return localPosition;
}

DirectX::XMFLOAT3 Node::GetLocalRotationEuler() const
{
    return QuaternionToEulerAnglesInternal(localRotationQuaternion); // Converts stored quaternion
}

DirectX::XMFLOAT4 Node::GetLocalRotationQuaternion() const
{
    return localRotationQuaternion;
}

DirectX::XMFLOAT3 Node::GetLocalScale() const
{
    return localScale;
}

// --- Directional Vectors (Forward, Right, Up, etc.) ---
// These remain unchanged as they rely on GetWorldTransform(), which is now correct.
Vector3 Node::Forward() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localForward = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // +Z in local space
    dx::XMVECTOR worldForward = dx::XMVector3TransformNormal(localForward, worldMat);
    worldForward = dx::XMVector3Normalize(worldForward);
    Vector3 result;
    dx::XMStoreFloat3(&result, worldForward);
    return result;
}

Vector3 Node::Back() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localBack = dx::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f); // -Z
    dx::XMVECTOR worldBack = dx::XMVector3TransformNormal(localBack, worldMat);
    worldBack = dx::XMVector3Normalize(worldBack);
    Vector3 result;
    dx::XMStoreFloat3(&result, worldBack);
    return result;
}

Vector3 Node::Right() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localRight = dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); // +X
    dx::XMVECTOR worldRight = dx::XMVector3TransformNormal(localRight, worldMat);
    worldRight = dx::XMVector3Normalize(worldRight);
    Vector3 result;
    dx::XMStoreFloat3(&result, worldRight);
    return result;
}

Vector3 Node::Left() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localLeft = dx::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f); // -X
    dx::XMVECTOR worldLeft = dx::XMVector3TransformNormal(localLeft, worldMat);
    worldLeft = dx::XMVector3Normalize(worldLeft);
    Vector3 result;
    dx::XMStoreFloat3(&result, worldLeft);
    return result;
}

Vector3 Node::Up() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localUp = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // +Y
    dx::XMVECTOR worldUp = dx::XMVector3TransformNormal(localUp, worldMat);
    worldUp = dx::XMVector3Normalize(worldUp);
    Vector3 result;
    dx::XMStoreFloat3(&result, worldUp);
    return result;
}

Vector3 Node::Down() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localDown = dx::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f); // -Y
    dx::XMVECTOR worldDown = dx::XMVector3TransformNormal(localDown, worldMat);
    worldDown = dx::XMVector3Normalize(worldDown);
    Vector3 result;
    dx::XMStoreFloat3(&result, worldDown);
    return result;
}

// --- World Transform Methods ---
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
    DirectX::XMMATRIX worldMat = GetWorldTransform(); // Ensures matrix is up-to-date
    DirectX::XMFLOAT3 worldPos;
    // The position is in the 4th row (r[3]) of a DirectX matrix
    dx::XMStoreFloat3(&worldPos, worldMat.r[3]);
    return worldPos;
}

void Node::SetWorldPosition(const DirectX::XMFLOAT3& worldPos)
{
    if (parent == nullptr)
    {
        SetLocalPosition(worldPos); // For root node, world position is local position
    }
    else
    {
        dx::XMMATRIX parentWorldTransform = parent->GetWorldTransform();
        dx::XMMATRIX invParentWorldTransform = dx::XMMatrixInverse(nullptr, parentWorldTransform);

        dx::XMVECTOR worldPosVec = dx::XMLoadFloat3(&worldPos);
        // Transform the desired world position into the parent's local space to find our new local position
        dx::XMVECTOR localPosVec = dx::XMVector3TransformCoord(worldPosVec, invParentWorldTransform);

        DirectX::XMFLOAT3 newLocalPos;
        dx::XMStoreFloat3(&newLocalPos, localPosVec);
        SetLocalPosition(newLocalPos);
    }
}

void Node::UpdateWorldTransform()
{
    dx::XMMATRIX finalLocalTransform = GetLocalTransform(); // Ensures local is up-to-date

    if (parent)
    {
        dx::XMMATRIX parentWorld = parent->GetWorldTransform(); // Ensures parent is up-to-date
        dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixMultiply(finalLocalTransform, parentWorld));
    }
    else
    {
        dx::XMStoreFloat4x4(&worldTransform, finalLocalTransform); // Root node
    }
    worldTransformDirty = false;

    // When this node's world transform changes, all its children's world transforms are now dirty
    for (auto& child : children)
    {
        child->worldTransformDirty = true;
    }
}

// --- Update & Draw ---
void Node::Update(float dt)
{
    // Ensure transforms are up-to-date before components or children use them
    // If local is dirty, world will be updated by UpdateLocalTransformFromComponents -> UpdateWorldTransform path or by direct call.
    if (localTransformDirty) { // This will also set worldTransformDirty = true
        UpdateLocalTransformFromComponents(); // Rebuilds local, marks world dirty
    }
    if (worldTransformDirty) { // If local was clean but world was dirty (e.g. parent moved)
        UpdateWorldTransform();         // Rebuilds world
    }
    // At this point, GetWorldTransform() will return an up-to-date matrix.

    for (auto& comp : components) {
        comp->Update(dt);
    }
    for (auto& child : children) {
        child->Update(dt);
    }
}

void Node::Submit(FrameCommander& frame, Graphics& gfx) const
{
    if (auto* modelComp = GetComponent<ModelComponent>())
    {
        modelComp->Submit(frame, gfx, GetWorldTransform());
    }
    // Original Submit for children was commented out, keeping it that way:
    //for (const auto& child : children)
    //{
    //    child->Submit(frame, gfx);
    //}
}

// --- ShowNodeTree (unchanged from original provided snippet) ---
void Node::ShowNodeTree(Node*& pSelectedNode) noexcept
{
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

// --- FindByTag and GetRoot (unchanged from original provided snippet) ---
Node* Node::GetRoot() const
{
    const Node* pCurrent = this;
    while (pCurrent->parent != nullptr)
    {
        pCurrent = pCurrent->parent;
    }
    return const_cast<Node*>(pCurrent);
}

Node* Node::FindFirstChildByTag(const std::string& searchTag)
{
    if (this->tag == searchTag)
    {
        return this;
    }
    for (const auto& child : children)
    {
        if (child)
        {
            Node* found = child->FindFirstChildByTag(searchTag);
            if (found != nullptr)
            {
                return found;
            }
        }
    }
    return nullptr;
}

// Internal recursive helper (unchanged logic)
void FindAllChildrenByTagRecursiveInternal(Node* currentNode, const std::string& searchTag, std::vector<Node*>& foundNodes)
{
    if (!currentNode) return;

    if (currentNode->tag == searchTag)
    {
        foundNodes.push_back(currentNode);
    }
    for (const auto& child : currentNode->GetChildren())
    {
        FindAllChildrenByTagRecursiveInternal(child.get(), searchTag, foundNodes);
    }
}

std::vector<Node*> Node::FindAllChildrenByTag(const std::string& searchTag)
{
    std::vector<Node*> foundNodes;
    // The original implementation started searching from children of `this` node.
    // If you want to include `this` node itself in the search if it matches the tag,
    // and then its descendants, the recursive helper should be called on `this`.
    // For now, matching the original behavior of only searching children:
    for (const auto& child : children) {
        FindAllChildrenByTagRecursiveInternal(child.get(), searchTag, foundNodes); // Call the renamed internal helper
    }
    return foundNodes;
}