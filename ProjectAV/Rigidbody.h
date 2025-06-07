#pragma once
#include <SimpleMath.h>
#include "PhysicsCommon.h"

using namespace DirectX::SimpleMath;
using namespace JPH;

class Trigger;
class PhysicsCommon;

class Rigidbody : public Component
{
public:
	Rigidbody() : Component(nullptr) { isRigidbody = true; }
	~Rigidbody();
	Rigidbody(Node* owner, BodyCreationSettings bodySettings);

	void Update(float dt) override;
	void ConnectWithOtherBody(BodyID other, Vec3 otherConnectPointLocal);
	void DisconnectConnectedBody();


	BodyID& GetBodyID();
private:
	BodyID bodyID;
	Constraint* constraint;
	float lastTest = 0.0f;
	bool firstRun = true;
};