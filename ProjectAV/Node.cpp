#include "Node.h"
#include "ModelComponent.h" 
#include "Graphics.h"       
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include <cmath> // For std::atan2, std::asin, std::copysign
#include "Rigidbody.h"

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
    transformationOutsidePhysicsTriggered = true;
}


// --- Hierarchy and Component methods remain the same ---
void Node::AddChild(std::unique_ptr<Node> pChild) { /* ... same ... */
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



// --- SetLocal... Methods ---

void Node::SetWorldPosition(const DirectX::XMFLOAT3& worldPos)
{
    transformationOutsidePhysicsTriggered = true;
    /*if (GetComponent<Rigidbody>() != nullptr)
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(GetComponent<Rigidbody>()->GetBodyID(), RVec3Arg(worldPos.x, worldPos.y, worldPos.z), EActivation::Activate);
    }*/
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

void Node::PhysicsSetWorldPosition(const DirectX::XMFLOAT3& worldPos)
{
    /*if (GetComponent<Rigidbody>() != nullptr)
    {
        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(GetComponent<Rigidbody>()->GetBodyID(), RVec3Arg(worldPos.x, worldPos.y, worldPos.z), EActivation::Activate);
    }*/
    if (parent == nullptr)
    {
        PhysicsSetLocalPosition(worldPos); // For root node, world position is local position
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
        PhysicsSetLocalPosition(newLocalPos);
    }
}

void Node::SetLocalTransform(DirectX::FXMMATRIX transform)
{
    dx::XMStoreFloat4x4(&localTransform, transform);
    UpdateStoredComponentsFromMatrix(); // Update stored pos, quat, scale from the new matrix
    localTransformDirty = false;
    worldTransformDirty = true;
}

void Node::SetLocalPosition(const DirectX::XMFLOAT3& pos)
{
    transformationOutsidePhysicsTriggered = true;
    if (Vector3(pos) != Vector3(localPosition))
    {
        /*if (GetComponent<Rigidbody>() != nullptr)
        {
            dx::XMMATRIX matS = dx::XMMatrixScaling(localScale.x, localScale.y, localScale.z);
            dx::XMMATRIX matR = dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&localRotationQuaternion));
            dx::XMMATRIX matT = dx::XMMatrixTranslation(pos.x, pos.y, pos.z);

            dx::XMMATRIX newComputedLocalTransform = matS * matR * matT;

            dx::XMMATRIX newComputedWorldTransform;
            if (parent)
            {
                dx::XMMATRIX parentWorldMatrix = parent->GetWorldTransform();
                newComputedWorldTransform = dx::XMMatrixMultiply(newComputedLocalTransform, parentWorldMatrix);
            }
            else
            {
                newComputedWorldTransform = newComputedLocalTransform;
            }

            DirectX::XMFLOAT3 newWorldPosFloat3;
            dx::XMStoreFloat3(&newWorldPosFloat3, newComputedWorldTransform.r[3]);

            RVec3Arg joltPhysicsPosition(newWorldPosFloat3.x, newWorldPosFloat3.y, newWorldPosFloat3.z);

            JPH::BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
            bodyInterface.SetPosition(GetComponent<Rigidbody>()->GetBodyID(), joltPhysicsPosition, JPH::EActivation::Activate);
        }*/
        localPosition = pos;
        localTransformDirty = true;
        worldTransformDirty = true;
    }
}

void Node::PhysicsSetLocalPosition(const DirectX::XMFLOAT3& pos)
{
    if (Vector3(pos) != Vector3(localPosition))
    {
        localPosition = pos;
        localTransformDirty = true;
        worldTransformDirty = true;
    }
}

// rotRad is Pitch, Yaw, Roll in Radians
void Node::SetLocalRotation(const DirectX::XMFLOAT3& rotRad)
{
    dx::XMVECTOR q = dx::XMQuaternionRotationRollPitchYaw(rotRad.x, rotRad.y, rotRad.z);
    dx::XMStoreFloat4(&localRotationQuaternion, q);
    localTransformDirty = true;
    worldTransformDirty = true;

    transformationOutsidePhysicsTriggered = true;

    // Ustaw rotacjê rigidbody, jeœli istnieje
    //if (GetComponent<Rigidbody>() != nullptr)
    //{
    //    // Przelicz na world transform, aby uzyskaæ rotacjê w œwiecie
    //    dx::XMMATRIX matS = dx::XMMatrixScaling(localScale.x, localScale.y, localScale.z);
    //    dx::XMMATRIX matR = dx::XMMatrixRotationQuaternion(q);
    //    dx::XMMATRIX matT = dx::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);

    //    dx::XMMATRIX newComputedLocalTransform = matS * matR * matT;
    //    dx::XMMATRIX newComputedWorldTransform;
    //    if (parent)
    //    {
    //        dx::XMMATRIX parentWorldMatrix = parent->GetWorldTransform();
    //        newComputedWorldTransform = dx::XMMatrixMultiply(newComputedLocalTransform, parentWorldMatrix);
    //    }
    //    else
    //    {
    //        newComputedWorldTransform = newComputedLocalTransform;
    //    }

    //    // Wyci¹gnij rotacjê z macierzy œwiata
    //    dx::XMVECTOR s, r, t;
    //    dx::XMMatrixDecompose(&s, &r, &t, newComputedWorldTransform);
    //    DirectX::XMFLOAT4 worldQuat;
    //    dx::XMStoreFloat4(&worldQuat, r);

    //    auto& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
    //    bodyInterface.SetRotation(GetComponent<Rigidbody>()->GetBodyID(), JPH::Quat(worldQuat.x, worldQuat.y, worldQuat.z, worldQuat.w), JPH::EActivation::Activate);
    //}
}

void Node::PhysicsSetLocalRotation(const DirectX::XMFLOAT4& quat)
{
    localRotationQuaternion = quat;
    localTransformDirty = true;
    worldTransformDirty = true;
}

void Node::SetLocalRotation(const DirectX::XMFLOAT4& quat)
{
    localRotationQuaternion = quat;
    localTransformDirty = true;
    worldTransformDirty = true;
    transformationOutsidePhysicsTriggered = true;

    // Ustaw rotacjê rigidbody, jeœli istnieje
    /*if (GetComponent<Rigidbody>() != nullptr)
    {
        dx::XMVECTOR q = dx::XMLoadFloat4(&quat);

        dx::XMMATRIX matS = dx::XMMatrixScaling(localScale.x, localScale.y, localScale.z);
        dx::XMMATRIX matR = dx::XMMatrixRotationQuaternion(q);
        dx::XMMATRIX matT = dx::XMMatrixTranslation(localPosition.x, localPosition.y, localPosition.z);

        dx::XMMATRIX newComputedLocalTransform = matS * matR * matT;
        dx::XMMATRIX newComputedWorldTransform;
        if (parent)
        {
            dx::XMMATRIX parentWorldMatrix = parent->GetWorldTransform();
            newComputedWorldTransform = dx::XMMatrixMultiply(newComputedLocalTransform, parentWorldMatrix);
        }
        else
        {
            newComputedWorldTransform = newComputedLocalTransform;
        }

        dx::XMVECTOR s, r, t;
        dx::XMMatrixDecompose(&s, &r, &t, newComputedWorldTransform);
        DirectX::XMFLOAT4 worldQuat;
        dx::XMStoreFloat4(&worldQuat, r);

        auto& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
        bodyInterface.SetRotation(GetComponent<Rigidbody>()->GetBodyID(), JPH::Quat(worldQuat.x, worldQuat.y, worldQuat.z, worldQuat.w), JPH::EActivation::Activate);
    }*/
}

void Node::SetLocalScale(const DirectX::XMFLOAT3& scale)
{
    localScale = scale;
    localTransformDirty = true;
    worldTransformDirty = true;
    transformationOutsidePhysicsTriggered = true;
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
        const_cast<Node*>(this)->UpdateLocalTransformFromComponents(transformationOutsidePhysicsTriggered);
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
        const_cast<Node*>(this)->UpdateWorldTransform(transformationOutsidePhysicsTriggered);
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



void Node::UpdateWorldTransform(bool transformationOutsidePhysicsTriggered)
{
    this->transformationOutsidePhysicsTriggered = transformationOutsidePhysicsTriggered;
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
    //if (GetComponent<Rigidbody>() != nullptr)
    //{
    //    // Wyci¹gnij pozycjê i rotacjê z worldTransform
    //    DirectX::XMMATRIX worldMat = dx::XMLoadFloat4x4(&worldTransform);
    //    DirectX::XMFLOAT3 pos;
    //    DirectX::XMFLOAT4 rot;
    //    dx::XMVECTOR s, r, t;
    //    dx::XMMatrixDecompose(&s, &r, &t, worldMat);
    //    dx::XMStoreFloat3(&pos, t);
    //    dx::XMStoreFloat4(&rot, r);

    //    auto& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
    //    bodyInterface.SetPosition(GetComponent<Rigidbody>()->GetBodyID(), JPH::RVec3(pos.x, pos.y, pos.z), JPH::EActivation::Activate);
    //    bodyInterface.SetRotation(GetComponent<Rigidbody>()->GetBodyID(), JPH::Quat(rot.x, rot.y, rot.z, rot.w), JPH::EActivation::Activate);
    //}
    worldTransformDirty = false;

    // When this node's world transform changes, all its children's world transforms are now dirty
    for (auto& child : children)
    {
        child->worldTransformDirty = true;
        child->transformationOutsidePhysicsTriggered = transformationOutsidePhysicsTriggered;
    }
}

void Node::UpdateLocalTransformFromComponents(bool transformationOutsidePhysicsTriggered)
{
    this->transformationOutsidePhysicsTriggered = transformationOutsidePhysicsTriggered;
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

// --- Update & Draw ---
void Node::Update(float dt)
{
    // Ensure transforms are up-to-date before components or children use them
    // If local is dirty, world will be updated by UpdateLocalTransformFromComponents -> UpdateWorldTransform path or by direct call.
    if (localTransformDirty) { // This will also set worldTransformDirty = true
        UpdateLocalTransformFromComponents(transformationOutsidePhysicsTriggered); // Rebuilds local, marks world dirty
    }
    if (worldTransformDirty) { // If local was clean but world was dirty (e.g. parent moved)
        UpdateWorldTransform(transformationOutsidePhysicsTriggered);         // Rebuilds world
    }
    // At this point, GetWorldTransform() will return an up-to-date matrix.

    //dx::XMFLOAT4X4 tempLocalTransform = localTransform;
    //dx::XMFLOAT4X4 tempWorldTransform = worldTransform;

    for (auto& comp : components) {
        if (transformationOutsidePhysicsTriggered)
        {
            if (comp->isRigidbody)
            {
                auto& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
                bodyInterface.SetPosition(GetComponent<Rigidbody>()->GetBodyID(), JPH::RVec3(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z), JPH::EActivation::Activate);
                bodyInterface.SetRotation(GetComponent<Rigidbody>()->GetBodyID(), JPH::Quat(GetLocalRotationQuaternion().x, GetLocalRotationQuaternion().y, GetLocalRotationQuaternion().z, GetLocalRotationQuaternion().w), JPH::EActivation::Activate);
                continue;
            }
        }
        comp->Update(dt);
    }

    //if (transformationOutsidePhysicsTriggered)
    //{
    //    localTransform = tempLocalTransform;
    //    worldTransform = tempWorldTransform;

    //    if (GetComponent<Rigidbody>() != nullptr)
    //    {
    //        myCounter++;
    //        if (GetName() == "Island B1")
    //        {
    //            OutputDebugString("kaunter: ");
    //            OutputDebugString(std::to_string(myCounter).c_str());
    //            OutputDebugString("\n");
    //        }
    //        // Wyci¹gnij pozycjê i rotacjê z worldTransform
    //        DirectX::XMMATRIX worldMat = dx::XMLoadFloat4x4(&worldTransform);
    //        DirectX::XMFLOAT3 worldPos;
    //        // The position is in the 4th row (r[3]) of a DirectX matrix
    //        dx::XMStoreFloat3(&worldPos, worldMat.r[3]);

    //        if (GetName() == "Stone")
    //        {
    //            OutputDebugString("\n");
    //            OutputDebugString("body id: ");
    //            OutputDebugString(std::to_string(GetComponent<Rigidbody>()->GetBodyID().GetIndex()).c_str());
    //            OutputDebugString(" ");
    //            OutputDebugString(std::to_string(worldPos.x).c_str());
    //            OutputDebugString(" ");
    //            OutputDebugString(std::to_string(worldPos.y).c_str());
    //            OutputDebugString(" ");
    //            OutputDebugString(std::to_string(worldPos.z).c_str());
    //            OutputDebugString(" ");
    //            OutputDebugString("\n");
    //        }

    //        auto& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
    //        bodyInterface.SetPosition(GetComponent<Rigidbody>()->GetBodyID(), JPH::RVec3(worldPos.x, worldPos.y, worldPos.z), JPH::EActivation::Activate);
    //        //bodyInterface.SetRotation(GetComponent<Rigidbody>()->GetBodyID(), JPH::Quat(rot.x, rot.y, rot.z, rot.w), JPH::EActivation::Activate);
    //    }
    //}

    for (auto& child : children) {
        child->Update(dt);
    }

    transformationOutsidePhysicsTriggered = false;
}

void Node::Submit(Graphics& gfx) const
{
    if (auto* modelComp = GetComponent<ModelComponent>())
    {
        modelComp->Submit(gfx, GetWorldTransform());
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

    return foundNodes; // Return the collected nodes
}
void Node::Destroy()
{
    markedForDestruction = true;
    // Recursively mark all children for destruction as well
    for (const auto& child : children)
    {
        if (child)
        {
            child->Destroy();
        }
    }

    //ClearComponents();
    // Note: This node itself doesn't get removed from its parent's list here.
    // That needs to be handled by the parent or a scene manager.
    // If it has a parent, it could notify the parent (more complex).
    // if (parent) {
    //    parent->RequestChildRemoval(this); // Would require new method in Node
    // }

}

bool Node::IsMarkedForDestruction() const
{
    return markedForDestruction;
}

// Helper to get a non-const reference to children vector for removal
std::vector<std::unique_ptr<Node>>& Node::GetChildren_NonConst()
{
    return children;
}

// Helper method for a parent to remove a specific child.
// This would typically be called by a scene management system.
void Node::RemoveChild(Node* childToRemove)
{
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [&](const std::unique_ptr<Node>& pChild) {
                return pChild.get() == childToRemove;
            }
        ),
        children.end()
    );
    // The unique_ptr going out of scope will call the child's destructor.
}

void Node::ClearComponents() 
{

    for (auto& comp : components) {
        if (comp) {
            comp.reset(); // Explicitly destroy the component (calls destructor)
        }
    }
    components.clear();
}