#include "Node.h"
#include "ModelComponent.h" 
#include "Graphics.h"       
#include <DirectXMath.h>
#include "imgui/imgui.h"
#include <cmath> 
#include "Rigidbody.h"


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

void Node::SetParent(Node* newParent)
{
    if (parent == newParent)
        return;

    if (parent)
    {
        auto& siblings = parent->GetChildren_NonConst();
        auto it = std::find_if(siblings.begin(), siblings.end(),
            [this](const std::unique_ptr<Node>& n) { return n.get() == this; });
        if (it != siblings.end())
        {
            std::unique_ptr<Node> thisNode = std::move(*it);
            siblings.erase(it);

            if (newParent)
            {
                newParent->AddChild(std::move(thisNode));
            }
        }
    }
    else if (newParent)
    {
        newParent->AddChild(std::unique_ptr<Node>(this));
    }

    parent = newParent;

    localTransformDirty = true;
    worldTransformDirty = true;
    transformationOutsidePhysicsTriggered = true;
}

Component* Node::AddComponent(std::unique_ptr<Component> pComponent) {
    assert(pComponent);
    components.push_back(std::move(pComponent));
    return components.back().get();
}
const std::vector<std::unique_ptr<Component>>& Node::GetComponents() const { return components; }


DirectX::XMFLOAT3 QuaternionToEulerAnglesInternal(DirectX::XMFLOAT4 q)
{
    DirectX::XMFLOAT3 angles;


    double sinr_cosp = 2.0 * (q.w * q.z + q.x * q.y);
    double cosr_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
    angles.z = static_cast<float>(std::atan2(sinr_cosp, cosr_cosp));


    double sinp = 2.0 * (q.w * q.x - q.y * q.z);
    if (std::abs(sinp) >= 1.0)
        angles.x = static_cast<float>(std::copysign(DirectX::XM_PI / 2.0, sinp)); 
    else
        angles.x = static_cast<float>(std::asin(sinp));


    double siny_cosp = 2.0 * (q.w * q.y + q.z * q.x);
    double cosy_cosp = 1.0 - 2.0 * (q.x * q.x + q.y * q.y);
    angles.y = static_cast<float>(std::atan2(siny_cosp, cosy_cosp));

    return { angles.x, angles.y, angles.z };
}


void Node::SetWorldPosition(const DirectX::XMFLOAT3& worldPos)
{
    transformationOutsidePhysicsTriggered = true;

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

        DirectX::XMFLOAT3 newLocalPos;
        dx::XMStoreFloat3(&newLocalPos, localPosVec);
        SetLocalPosition(newLocalPos);
    }
}

void Node::PhysicsSetWorldPosition(const DirectX::XMFLOAT3& worldPos)
{
    if (parent == nullptr)
    {
        PhysicsSetLocalPosition(worldPos); 
    }
    else
    {
        dx::XMMATRIX parentWorldTransform = parent->GetWorldTransform();
        dx::XMMATRIX invParentWorldTransform = dx::XMMatrixInverse(nullptr, parentWorldTransform);

        dx::XMVECTOR worldPosVec = dx::XMLoadFloat3(&worldPos);

        dx::XMVECTOR localPosVec = dx::XMVector3TransformCoord(worldPosVec, invParentWorldTransform);

        DirectX::XMFLOAT3 newLocalPos;
        dx::XMStoreFloat3(&newLocalPos, localPosVec);
        PhysicsSetLocalPosition(newLocalPos);
    }
}

void Node::SetLocalTransform(DirectX::FXMMATRIX transform)
{
    dx::XMStoreFloat4x4(&localTransform, transform);
    UpdateStoredComponentsFromMatrix(); 
    localTransformDirty = false;
    worldTransformDirty = true;
}

void Node::SetLocalPosition(const DirectX::XMFLOAT3& pos)
{
    transformationOutsidePhysicsTriggered = true;
    if (Vector3(pos) != Vector3(localPosition))
    {
        
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


void Node::SetLocalRotation(const DirectX::XMFLOAT3& rotRad)
{
    dx::XMVECTOR q = dx::XMQuaternionRotationRollPitchYaw(rotRad.x, rotRad.y, rotRad.z);
    dx::XMStoreFloat4(&localRotationQuaternion, q);
    localTransformDirty = true;
    worldTransformDirty = true;

    transformationOutsidePhysicsTriggered = true;

    
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

   
}

void Node::SetWorldRotation(const DirectX::XMFLOAT4& worldQuat)
{
    transformationOutsidePhysicsTriggered = true; 

    if (parent == nullptr)
    {
       
        SetLocalRotation(worldQuat);
    }
    else
    {
       
        dx::XMFLOAT4 parentWorldRotationQuat = parent->GetWorldRotationQuaternion();
        dx::XMVECTOR parentWorldRotVec = dx::XMLoadFloat4(&parentWorldRotationQuat);

        
        dx::XMVECTOR invParentWorldRotVec = dx::XMQuaternionInverse(parentWorldRotVec);

        
        dx::XMVECTOR desiredWorldRotVec = dx::XMLoadFloat4(&worldQuat);

        
        dx::XMVECTOR newLocalRotVec = dx::XMQuaternionMultiply(desiredWorldRotVec, invParentWorldRotVec);
        newLocalRotVec = dx::XMQuaternionNormalize(newLocalRotVec); 

        DirectX::XMFLOAT4 newLocalQuat;
        dx::XMStoreFloat4(&newLocalQuat, newLocalRotVec);
        SetLocalRotation(newLocalQuat); 
    }
}

void Node::PhysicsSetWorldRotation(const DirectX::XMFLOAT4& worldQuat)
{
    if (parent == nullptr)
    {
        
        PhysicsSetLocalRotation(worldQuat);
    }
    else
    {
        
        dx::XMFLOAT4 parentWorldRotationQuat = parent->GetWorldRotationQuaternion();
        dx::XMVECTOR parentWorldRotVec = dx::XMLoadFloat4(&parentWorldRotationQuat);


        dx::XMVECTOR invParentWorldRotVec = dx::XMQuaternionInverse(parentWorldRotVec);


        dx::XMVECTOR desiredWorldRotVec = dx::XMLoadFloat4(&worldQuat);


        dx::XMVECTOR newLocalRotVec = dx::XMQuaternionMultiply(desiredWorldRotVec, invParentWorldRotVec);
        newLocalRotVec = dx::XMQuaternionNormalize(newLocalRotVec);

        DirectX::XMFLOAT4 newLocalQuat;
        dx::XMStoreFloat4(&newLocalQuat, newLocalRotVec);
        PhysicsSetLocalRotation(newLocalQuat);
    }

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
    return QuaternionToEulerAnglesInternal(localRotationQuaternion); 
}

DirectX::XMFLOAT4 Node::GetLocalRotationQuaternion() const
{
    return localRotationQuaternion;
}

DirectX::XMFLOAT4 Node::GetWorldRotationQuaternion() const
{

    dx::XMMATRIX worldMat = GetWorldTransform();


    dx::XMVECTOR scaleVec;
    dx::XMVECTOR rotationQuatVec;
    dx::XMVECTOR translationVec;

   
    if (!dx::XMMatrixDecompose(&scaleVec, &rotationQuatVec, &translationVec, worldMat))
    {
        
        return { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    DirectX::XMFLOAT4 worldRotationQuat;
    dx::XMStoreFloat4(&worldRotationQuat, rotationQuatVec);

    return worldRotationQuat;
}

DirectX::XMFLOAT3 Node::GetLocalScale() const
{
    return localScale;
}


Vector3 Node::Forward() const
{
    dx::XMMATRIX worldMat = GetWorldTransform();
    dx::XMVECTOR localForward = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); 
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
    DirectX::XMMATRIX worldMat = GetWorldTransform();
    DirectX::XMFLOAT3 worldPos;
    dx::XMStoreFloat3(&worldPos, worldMat.r[3]);
    return worldPos;
}



void Node::UpdateWorldTransform(bool transformationOutsidePhysicsTriggered)
{
    this->transformationOutsidePhysicsTriggered = transformationOutsidePhysicsTriggered;
    dx::XMMATRIX finalLocalTransform = GetLocalTransform();

    if (parent)
    {
        dx::XMMATRIX parentWorld = parent->GetWorldTransform();
        dx::XMStoreFloat4x4(&worldTransform, dx::XMMatrixMultiply(finalLocalTransform, parentWorld));
    }
    else
    {
        dx::XMStoreFloat4x4(&worldTransform, finalLocalTransform); 
    }
    
    worldTransformDirty = false;

    
    for (auto& child : children)
    {
        child->worldTransformDirty = true;
        if (transformationOutsidePhysicsTriggered)
        {
            child->transformationOutsidePhysicsTriggered = transformationOutsidePhysicsTriggered;
        }
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

        
        dx::XMStoreFloat4x4(&localTransform, matS * matR * matT);

        localTransformDirty = false;
        worldTransformDirty = true;
    }
}


void Node::UpdateStoredComponentsFromMatrix()
{
    dx::XMMATRIX matrix = dx::XMLoadFloat4x4(&localTransform);
    dx::XMVECTOR s_vec, r_quat_vec, t_vec;
    dx::XMMatrixDecompose(&s_vec, &r_quat_vec, &t_vec, matrix);

    dx::XMStoreFloat3(&localPosition, t_vec);
    dx::XMStoreFloat4(&localRotationQuaternion, r_quat_vec);
    dx::XMStoreFloat3(&localScale, s_vec);
}


void Node::Update(float dt)
{
    if (this == NULL || this == nullptr) return;
    if (localTransformDirty) { 
        UpdateLocalTransformFromComponents(transformationOutsidePhysicsTriggered); 
    }
    if (worldTransformDirty) { 
        UpdateWorldTransform(transformationOutsidePhysicsTriggered);      
    }
   

    for (auto& comp : components) {
        if (transformationOutsidePhysicsTriggered)
        {
            if (comp->isRigidbody)
            {
                
                
                auto& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
                bodyInterface.SetPosition(GetComponent<Rigidbody>()->GetBodyID(), JPH::RVec3(GetWorldPosition().x, GetWorldPosition().y, GetWorldPosition().z), JPH::EActivation::Activate);
                bodyInterface.SetRotation(GetComponent<Rigidbody>()->GetBodyID(), JPH::Quat(GetWorldRotationQuaternion().x, GetWorldRotationQuaternion().y, GetWorldRotationQuaternion().z, GetWorldRotationQuaternion().w), JPH::EActivation::Activate);
                continue;
            }
        }
        comp->Update(dt);
    }

   

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
    
}

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

    for (const auto& child : children) {
        FindAllChildrenByTagRecursiveInternal(child.get(), searchTag, foundNodes); 
    }

    return foundNodes; 
}
void Node::Destroy()
{
    if(!markedForDestruction)
    {
        if (GetComponent<Rigidbody>() != nullptr)
        {
            dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->RemoveRigidbodyData(GetComponent<Rigidbody>()->GetBodyID());
        }
        if (GetComponent<Trigger>() != nullptr)
        {
            dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->RemoveTriggerData(GetComponent<Trigger>()->GetBodyID());
        }
    }
    markedForDestruction = true;

    for (const auto& child : children)
    {
        if (child)
        {
            child->Destroy();
        }
    }



}

bool Node::IsMarkedForDestruction() const
{
    return markedForDestruction;
}


std::vector<std::unique_ptr<Node>>& Node::GetChildren_NonConst()
{
    return children;
}


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
   
}

void Node::ClearComponents() 
{

    for (auto& comp : components) {
        if (comp) {
            comp.reset(); 
        }
    }
    components.clear();
}