#include "FireStatue.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
FireStatue::FireStatue(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void FireStatue::Update(float dt)
{
    dx::XMFLOAT4 currentQuat = pOwner->GetLocalRotationQuaternion();

    dx::XMVECTOR spinQuat = dx::XMQuaternionRotationRollPitchYaw(0.0f, dt * spinSpeed, 0.0f);

    dx::XMVECTOR currentQuatVec = dx::XMLoadFloat4(&currentQuat);

    dx::XMVECTOR resultQuat = dx::XMQuaternionMultiply(spinQuat, currentQuatVec);

    dx::XMFLOAT4 newQuat;
    dx::XMStoreFloat4(&newQuat, resultQuat);

    pOwner->SetLocalRotation(newQuat);
}
