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

	float rotX, rotY, rotZ;
	trans.getRotation().getEulerZYX(rotZ, rotY, rotX);
	pOwner->SetLocalRotation(DirectX::XMFLOAT3(rotX, rotY, rotZ));
}

btRigidBody* Rigidbody::GetBulletRigidbody()
{
	return bulletRigidbody;
}
