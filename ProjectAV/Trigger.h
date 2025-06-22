#pragma once

#include "Rigidbody.h"
#include "PhysicsCommon.h"

class Rigidbody;
class PhysicsCommon;

using namespace JPH;

class Trigger : public Component
{
public:
	Trigger(Node* owner);
	
	Trigger(Node* owner, BodyCreationSettings bodySettings, bool parentHasRigidbody);

	void Update(float dt) override;

	BodyID GetBodyID();

private:
	BodyID bodyID;
	bool rigidbodyAssigned = false;
};