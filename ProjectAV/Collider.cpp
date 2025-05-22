//#include "Collider.h"
//
//void Collider::Update(float dt)
//{
//	btTransform transform;
//	transform.setIdentity();
//	transform.setOrigin(btVector3(pOwner->GetWorldPosition().x, 
//								  pOwner->GetWorldPosition().y,
//								  pOwner->GetWorldPosition().z));
//	btQuaternion rot;
//	rot.setEulerZYX(pOwner->GetLocalRotationEuler().z,
//					pOwner->GetLocalRotationEuler().y,
//					pOwner->GetLocalRotationEuler().x);
//	transform.setRotation(rot);
//
//	bulletCollisionObject->setWorldTransform(transform);
//}
//
//btCollisionObject* Collider::GetBulletCollisionObject()
//{
//	return bulletCollisionObject;
//}
