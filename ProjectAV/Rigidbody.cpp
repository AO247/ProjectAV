#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

void Rigidbody::Update(float dt)
{
	RVec3 pos = PhysicsCommon::physicsSystem->GetBodyInterface().GetCenterOfMassPosition(bodyID);
	Quat rot = PhysicsCommon::physicsSystem->GetBodyInterface().GetRotation(bodyID);

	pOwner->SetLocalPosition(DirectX::XMFLOAT3(pos.GetX(),
												pos.GetY(),
												pos.GetZ()));
	pOwner->SetLocalRotation(DirectX::XMFLOAT4(rot.GetX(),
												rot.GetY(),
												rot.GetZ(),
												rot.GetW()));
}

BodyID& Rigidbody::GetBodyID()
{
	return bodyID;
}
