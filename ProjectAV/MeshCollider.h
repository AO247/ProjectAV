#pragma once

#include "Collider.h"
#include "BoundingSphere.h"
#include "ModelComponent.h"

struct Projection
{
	float min, max;
};

class MeshCollider : public Collider
{
public:
	MeshCollider(Node* owner, Rigidbody* rigidbody, ModelComponent* modelComponent) : Collider(owner, Collider::ColliderTypes::MESH, rigidbody)
	{
		std::vector<Triangle> modelTriangles = modelComponent->GetTriangles(DirectX::XMMatrixIdentity());
		
		for (int i = 0; i < modelTriangles.size(); i++)
		{
			Triangle modelTriangle = modelTriangles[i];
			Vector3 modelTriangleA( modelTriangle.v0.x,
									modelTriangle.v0.y,
									modelTriangle.v0.z);
			Vector3 modelTriangleB( modelTriangle.v1.x,
									modelTriangle.v1.y,
									modelTriangle.v1.z);
			Vector3 modelTriangleC( modelTriangle.v2.x,
									modelTriangle.v2.y,
									modelTriangle.v2.z);

			MeshTriangle meshTriangle(modelTriangleA,
										modelTriangleB,
										modelTriangleC);

			triangles.push_back(meshTriangle);
		}
	}

	struct MeshTriangle
	{
		MeshTriangle(Vector3 a, Vector3 b, Vector3 c) : a(a), b(b), c(c) {}
		Vector3 a, b, c;
	};

	IntersectData IntersectMesh(MeshCollider* other);
	IntersectData IntersectSphere(BoundingSphere* other);
	IntersectData IntersectOBB(OBB* other);
	IntersectData IntersectCapsule(CapsuleCollider* other);
	Vector3 GetTransformedCenter();
	MeshTriangle GetTransformedMeshTriangle(MeshTriangle meshTriangle);

	IntersectData TriangleIntersectOBB(MeshTriangle triangle, OBB* obb);

	std::vector<MeshCollider::MeshTriangle> triangles;
private:
	Vector3 ClosestPointOnTriangle(Vector3 point, MeshCollider::MeshTriangle triangle);
	Projection ProjectOntoAxis(Vector3 axis, MeshCollider::MeshTriangle triangle);
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
	bool OverlapOnAxis(MeshCollider::MeshTriangle t1, MeshCollider::MeshTriangle t2, Vector3 axis);
};