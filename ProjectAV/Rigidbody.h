#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Collider.h"
#include "btBulletDynamicsCommon.h"

using namespace DirectX::SimpleMath;

class PhysicsEngine;
class Collider;

class Rigidbody : public Component
{
public:
	Rigidbody() : Component(nullptr) {}
	Rigidbody(Node* owner, Vector3 position, float mass, btCollisionShape* shape) : Component(owner)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		btVector3 localInertia(0, 0, 0);
		//if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
		bulletRigidbody = new btRigidBody(rbInfo);
		bulletRigidbody->setFriction(10.0f);
	}

	void Update(float dt) override;

	btRigidBody* GetBulletRigidbody();

private:
	btRigidBody* bulletRigidbody;
};