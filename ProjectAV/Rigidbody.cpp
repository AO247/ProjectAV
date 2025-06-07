#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

Rigidbody::~Rigidbody()
{
	if (constraint != nullptr)
	{
		PhysicsCommon::physicsSystem->RemoveConstraint(constraint);
		//delete constraint;
	}
}

Rigidbody::Rigidbody(Node* owner, BodyCreationSettings bodySettings) : Component(owner)
{
	isRigidbody = true;
	bodySettings.mPosition = RVec3(owner->GetWorldPosition().x, 
									owner->GetWorldPosition().y,
									owner->GetWorldPosition().z);
	bodySettings.mRotation = Quat(owner->GetLocalRotationQuaternion().x,
									owner->GetLocalRotationQuaternion().y,
									owner->GetLocalRotationQuaternion().z,
									owner->GetLocalRotationQuaternion().w);
	BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
	bodySettings.mUserData = reinterpret_cast<uint64>(owner);
	bodyID = bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate);
	dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->AddRigidbody(bodyID);
}

void Rigidbody::Update(float dt)
{
	RVec3 pos = PhysicsCommon::physicsSystem->GetBodyInterface().GetPosition(bodyID);
	Quat rot = PhysicsCommon::physicsSystem->GetBodyInterface().GetRotation(bodyID);
	pOwner->PhysicsSetWorldPosition(DirectX::XMFLOAT3(pos.GetX(),
		pos.GetY(),
		pos.GetZ()));
	pOwner->PhysicsSetWorldRotation(DirectX::XMFLOAT4(rot.GetX(),
		rot.GetY(),
		rot.GetZ(),
		rot.GetW()));
}

void Rigidbody::ConnectWithOtherBody(BodyID other, Vec3 otherConnectPointLocal)
{
	Body* body1 = PhysicsCommon::physicsSystem->GetBodyLockInterface().TryGetBody(bodyID);
	Body* body2 = PhysicsCommon::physicsSystem->GetBodyLockInterface().TryGetBody(other);

	FixedConstraintSettings settings;
	settings.mPoint1 = Vec3(0, 0, 0);
	settings.mPoint2 = Vec3(otherConnectPointLocal);

	constraint = settings.Create(*body1, *body2);
	PhysicsCommon::physicsSystem->AddConstraint(constraint);
}

void Rigidbody::DisconnectConnectedBody()
{
	if (constraint != nullptr)
	{
		PhysicsCommon::physicsSystem->RemoveConstraint(constraint);
		constraint = nullptr;
	}
	//delete constraint;
}

BodyID& Rigidbody::GetBodyID()
{
	return bodyID;
}
