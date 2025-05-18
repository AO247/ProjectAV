#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

void Rigidbody::Update(float dt)
{
	btTransform trans;
	bulletRigidbody->getMotionState()->getWorldTransform(trans);

	pOwner->SetLocalPosition(DirectX::XMFLOAT3( float(trans.getOrigin().getX()), 
												float(trans.getOrigin().getY()),
												float(trans.getOrigin().getZ())));

	btQuaternion rotationQuaternion = trans.getRotation();
	pOwner->SetLocalRotation(DirectX::XMFLOAT4(rotationQuaternion.x(), 
											   rotationQuaternion.y(),
											   rotationQuaternion.z(),
											   rotationQuaternion.w()));
}

btRigidBody* Rigidbody::GetBulletRigidbody()
{
	return bulletRigidbody;
}
