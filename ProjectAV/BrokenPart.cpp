#include "BrokenPart.h"
#include <string>
BrokenPart::BrokenPart(Node* owner) : Component(owner)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
}


void BrokenPart::OnCollisionEnter(Node* object)
{
	if (object == nullptr) return;
	if (pOwner->GetParent() != object && !broked && object->tag != "TRIGGER")
	{

		rigidbody->DisconnectConnectedBody();
		broked = true;
	}
}


void BrokenPart::DrawImGuiControls()
{
	Vec3 position = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
	float l = position.Length();
	ImGui::InputFloat("Velocity", &l);
	ImGui::Checkbox("Broked", &broked);


}
