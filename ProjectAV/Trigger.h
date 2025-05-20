#pragma once
#include "Component.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "SimpleMath.h"
#include "Node.h"

using namespace DirectX::SimpleMath;

class Trigger : public Component
{
public:
	Trigger(Node* owner, btCollisionShape* shape) : Component(owner)
	{
		bulletGhostObject = new btGhostObject();
		bulletGhostObject->setCollisionShape(shape);
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(pOwner->GetWorldPosition().x,
										pOwner->GetWorldPosition().y,
										pOwner->GetWorldPosition().z));
		bulletGhostObject->setWorldTransform(transform);
		bulletGhostObject->setUserPointer(owner);
	}

	std::vector<Node*> GetTriggerEnter();
	std::vector<Node*> GetTriggerExit();
	std::vector<Node*> GetTriggerStay();

	void Update(float dt) override;

private:
	btGhostObject* bulletGhostObject;

	std::vector<btCollisionObject*> objectsThatEnteredThisFrame;
	std::vector<btCollisionObject*> objectsThatExitedThisFrame;
	std::vector<btCollisionObject*> objectsPresentThisFrame;
	std::vector<btCollisionObject*> objectsPresentLastFrame;
};

