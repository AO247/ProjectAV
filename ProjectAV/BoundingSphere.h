#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"
#include "Collider.h"
#include "SolidSphere.h"
#include "AABB.h"
#include "OBB.h"
#include "CapsuleCollider.h"
#include "MeshCollider.h"

using namespace DirectX::SimpleMath;
namespace AxisAligned {
	class AABB;
}

class OBB;
class CapsuleCollider;
class MeshCollider;

class BoundingSphere : public Collider
{
public:
	BoundingSphere(Node* owner, Vector3 center, float radius, Rigidbody* rigidbody) :
		center(center),
		radius(radius),
		Collider(owner, Collider::ColliderTypes::SPHERE, rigidbody) {}

	IntersectData IntersectBoundingSphere(BoundingSphere* other);
	IntersectData IntersectAABB(AxisAligned::AABB* other);
	IntersectData IntersectOBB(OBB* other);
	IntersectData IntersectCapsule(CapsuleCollider* other);
	IntersectData IntersectMesh(MeshCollider* other);
	RaycastData IntersectRay(Raycast* ray) override;

	Vector3& GetCenter();
	float GetRadius();
	Vector3 GetTransformedCenter();
	Vector3 GetColliderTransformedCenter() override;

private:
	Vector3 center;
	float radius;
};