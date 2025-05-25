#include "Trigger.h"



Trigger::Trigger(Node* owner) : Component(owner)
{
}

Trigger::Trigger(Node* owner, BodyCreationSettings bodySettings, bool parentHasRigidbody) : Component(owner)
{
	rigidbodyAssigned = parentHasRigidbody;

	BodyInterface& bodyInterface = PhysicsCommon::physicsSystem->GetBodyInterface();
	bodySettings.mUserData = reinterpret_cast<uint64>(owner);
	bodySettings.mIsSensor = true;
	bodySettings.mMotionType = EMotionType::Kinematic;
	bodyID = bodyInterface.CreateAndAddBody(bodySettings, EActivation::Activate);
	OutputDebugString("triggercio utworzony i jego id to: ");
	OutputDebugString(std::to_string(bodyID.GetIndex()).c_str());
	OutputDebugString("\n");
	bodyInterface.ActivateBody(bodyID);

	if (parentHasRigidbody) 
	{
		RVec3 anchor = RVec3(owner->GetParent()->GetWorldPosition().x + owner->GetLocalPosition().x,
			owner->GetParent()->GetWorldPosition().y + owner->GetLocalPosition().y,
			owner->GetParent()->GetWorldPosition().z + owner->GetLocalPosition().z);
		Quat rot = Quat::sIdentity();
		Mat44 worldTransform = Mat44::sRotationTranslation(rot, anchor);

		Mat44 body1ToWorld = bodyInterface.GetWorldTransform(owner->GetParent()->GetComponent<Rigidbody>()->GetBodyID());
		Mat44 body2ToWorld = bodyInterface.GetWorldTransform(bodyID);

		RVec3 localPoint1 = body1ToWorld.Inversed().Multiply3x3(anchor - body1ToWorld.GetTranslation());
		RVec3 localPoint2 = body2ToWorld.Inversed().Multiply3x3(anchor - body2ToWorld.GetTranslation());

		FixedConstraintSettings* settings = new FixedConstraintSettings();
		settings->mPoint1 = localPoint1;
		settings->mPoint2 = localPoint2;

		PhysicsCommon::physicsSystem->AddConstraint(bodyInterface.CreateConstraint(settings, owner->GetParent()->GetComponent<Rigidbody>()->GetBodyID(), bodyID));
	}
	dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->AddTrigger(bodyID);
}

void Trigger::Update(float dt)
{
	if (!rigidbodyAssigned)
	{
		PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(bodyID, RVec3Arg(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z), EActivation::Activate);
	}
}

//void Trigger::AddContactPoint(Node* object)
//{
//	contacts[object]++;
//	if (contacts[object] == 1)
//	{
//		const auto& components = pOwner->GetComponents();
//		for (int i = 0; i < components.size(); i++)
//		{
//			components[i]->OnTriggerEnter(object);
//		}
//	}
//}
//
//void Trigger::RemoveContactPoint(Node* object)
//{
//	contacts[object]--;
//	if (contacts[object] == 0)
//	{
//		const auto& components = pOwner->GetComponents();
//		for (int i = 0; i < components.size(); i++)
//		{
//			components[i]->OnTriggerExit(object);
//		}
//	}
//}
