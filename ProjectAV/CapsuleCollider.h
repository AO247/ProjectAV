#pragma once
#include "Collider.h"
#include "BoundingSphere.h"

class BoundingSphere;
class OBB;

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(Node* owner, Rigidbody* rigidbody, float radius, Vector3 base, Vector3 tip) : Collider(owner, Collider::ColliderTypes::CAPSULE, rigidbody),
																								  radius(radius),
																								  base(base),
																								  tip(tip)
	{}

	IntersectData IntersectCapsule(CapsuleCollider* other);
	IntersectData IntersectSphere(BoundingSphere* other);
	IntersectData IntersectOBB(OBB* other);

	Vector3 GetTransformedBase();
	Vector3 GetTransformedTip();
	float GetRadius();
	Vector3 ClosestPointOnLineSegment(Vector3 a, Vector3 b, Vector3 point);
private:
	float radius;
	Vector3 base;
	Vector3 tip;

	
};

