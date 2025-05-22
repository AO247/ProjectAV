//#pragma once
//
//#include "Component.h"
//#include "btBulletDynamicsCommon.h"
//#include "Node.h"
//
//class Collider : public Component
//{
//public:
//	Collider(Node* owner, btCollisionShape* shape) : Component(owner) 
//	{
//		bulletCollisionObject = new btCollisionObject();
//		bulletCollisionObject->setCollisionShape(shape);
//	}
//
//	void Update(float dt) override;
//
//	btCollisionObject* GetBulletCollisionObject();
//private:
//	btCollisionObject* bulletCollisionObject;
//};
//
