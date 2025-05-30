#pragma once

#include "Rigidbody.h"
#include "PhysicsCommon.h"
//#include <map>

class Rigidbody;
class PhysicsCommon;

using namespace JPH;

//JPH::PhysicsSystem* PhysicsCommon::physicsSystem = nullptr;

class Trigger : public Component
{
public:
	Trigger(Node* owner);

	Trigger(Node* owner, BodyCreationSettings bodySettings, bool parentHasRigidbody);

	void Update(float dt) override;
	/*void AddContactPoint(Node* object);
	void RemoveContactPoint(Node* object);*/

	BodyID GetBodyID();

private:
	BodyID bodyID;
	bool rigidbodyAssigned = false;
	/*int contactPointCounter = 0;
	std::map<Node*, int> contacts;*/
};