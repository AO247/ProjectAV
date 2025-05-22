#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Collider.h"
#include "btBulletDynamicsCommon.h"
#include "PhysicsCommon.h"

using namespace DirectX::SimpleMath;
using namespace JPH;

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

	Rigidbody(Node* owner, Vector3 position, float mass, Shape* shape) : Component(owner)
	{
		BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
		BodyCreationSettings bodySettings(shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
		//bodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
		//bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(2.0f * Vec3(cTorusRadius, cTubeRadius, cTorusRadius), 1000.0f);
		bodyID = bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate);
	}

	Rigidbody(Node* owner, BodyCreationSettings bodySettings) : Component(owner)
	{
		BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
		bodyID = bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate);
	}

	void Update(float dt) override;

	btRigidBody* GetBulletRigidbody();
	BodyID& GetBodyID();

private:
	btRigidBody* bulletRigidbody;
	BodyID bodyID;
};