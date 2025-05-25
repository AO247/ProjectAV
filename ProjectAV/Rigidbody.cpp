#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

Rigidbody::Rigidbody(Node* owner, Vector3 position, float mass, Shape* shape) : Component(owner)
{
	BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
	BodyCreationSettings bodySettings(shape, RVec3(position.x, position.y, position.z), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	bodySettings.mUserData = reinterpret_cast<uint64>(owner);
	//bodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;
	//bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(2.0f * Vec3(cTorusRadius, cTubeRadius, cTorusRadius), 1000.0f);
	bodyID = bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate);
}

Rigidbody::Rigidbody(Node* owner, BodyCreationSettings bodySettings) : Component(owner)
{
	BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
	bodySettings.mUserData = reinterpret_cast<uint64>(owner);
	bodyID = bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate);
}

void Rigidbody::Update(float dt)
{
	RVec3 pos = PhysicsCommon::physicsSystem->GetBodyInterface().GetPosition(bodyID);
	Quat rot = PhysicsCommon::physicsSystem->GetBodyInterface().GetRotation(bodyID);
	pOwner->SetLocalPosition(DirectX::XMFLOAT3(pos.GetX(),
												pos.GetY(),
												pos.GetZ()));
	pOwner->SetLocalRotation(DirectX::XMFLOAT4(rot.GetX(),
												rot.GetY(),
												rot.GetZ(),
												rot.GetW()));
}

BodyID& Rigidbody::GetBodyID()
{
	return bodyID;
}
