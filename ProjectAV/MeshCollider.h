#pragma once

#include "Collider.h"
#include "BoundingSphere.h"

struct Triangle
{
	Triangle(Vector3 a, Vector3 b, Vector3 c) : a(b), b(b), c(c) {}
	Vector3 a, b, c;
};

struct Projection
{
	float min, max;
};

class MeshCollider : public Collider
{
public:
	MeshCollider(Node* owner, Rigidbody* rigidbody) : Collider(owner, Collider::ColliderTypes::MESH, rigidbody) {}

	IntersectData IntersectMesh(MeshCollider* other);
	IntersectData IntersectSphere(BoundingSphere* other);
	IntersectData IntersectOBB(OBB* other);
	IntersectData IntersectCapsule(CapsuleCollider* other);
	Vector3 GetTransformedCenter();

	std::vector<Triangle> triangles;
private:
	Vector3 ClosestPointOnTriangle(Vector3 point, Triangle triangle);
	Projection ProjectOntoAxis(Vector3 axis, Triangle triangle);
	float MinFloat(float a, float b, float c)
	{
		float result = a;
		if (b < a)
		{
			result = b;
		}
		if (c < result)
		{
			result = c;
		}
		return result;
	}
	float MaxFloat(float a, float b, float c)
	{
		float result = a;
		if (b > a)
		{
			result = b;
		}
		if (c > result)
		{
			result = c;
		}
		return result;
	}
	bool OverlapOnAxis(Triangle t1, Triangle t2, Vector3 axis);
};