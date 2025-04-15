#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"
#include "Collider.h"
#include "SolidSphere.h"
#include "AABB.h"

using namespace DirectX::SimpleMath;
namespace AxisAligned {
	class AABB;
}

class BoundingSphere : public Collider
{
public:
	BoundingSphere(Node* owner, Vector3 center, float radius, Rigidbody* rigidbody) :
		center(center),
		radius(radius),
		Collider(owner, Collider::ColliderTypes::SPHERE, rigidbody) {}

	IntersectData IntersectBoundingSphere(BoundingSphere* other);
	IntersectData IntersectAABB(AxisAligned::AABB* other);

	Vector3& GetCenter();
	float GetRadius();
	Vector3 GetTransformedCenter();

private:
	Vector3 center;
	float radius;
};