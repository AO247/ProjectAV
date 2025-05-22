#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

void Rigidbody::Update(float dt)
{
	/*btTransform trans;
	bulletRigidbody->getMotionState()->getWorldTransform(trans);

	pOwner->SetLocalPosition(DirectX::XMFLOAT3( float(trans.getOrigin().getX()), 
												float(trans.getOrigin().getY()),
												float(trans.getOrigin().getZ())));

	btQuaternion rotationQuaternion = trans.getRotation();
	pOwner->SetLocalRotation(DirectX::XMFLOAT4(rotationQuaternion.x(), 
											   rotationQuaternion.y(),
											   rotationQuaternion.z(),
											   rotationQuaternion.w()));*/

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

btRigidBody* Rigidbody::GetBulletRigidbody()
{
	return bulletRigidbody;
}

BodyID& Rigidbody::GetBodyID()
{
	return bodyID;
}
