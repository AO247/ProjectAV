#pragma once
#include <SimpleMath.h>
//#include "Component.h"
#include "PhysicsCommon.h"

using namespace DirectX::SimpleMath;
using namespace JPH;

class Trigger;
class PhysicsCommon;

class Rigidbody : public Component
{
public:
	Rigidbody() : Component(nullptr) {}

	Rigidbody(Node* owner, Vector3 position, float mass, Shape* shape);

	Rigidbody(Node* owner, BodyCreationSettings bodySettings);

	void Update(float dt) override;

	BodyID& GetBodyID();

private:
	BodyID bodyID;
};