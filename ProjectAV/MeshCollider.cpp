#include "MeshCollider.h"

IntersectData MeshCollider::IntersectMesh(MeshCollider* other)
{
	bool collided = false;
	float globalMinPenetration = FLT_MAX;
	Vector3 globalBestAxis;
	for (int i = 0; i < triangles.size(); i++)
	{
		for (int j = 0; j < other->triangles.size(); j++)
		{
			MeshCollider::MeshTriangle t1 = GetTransformedMeshTriangle(triangles[i]);
			MeshCollider::MeshTriangle t2 = other->GetTransformedMeshTriangle(other->triangles[j]);

			Vector3 e1a = t1.b - t1.a;
			Vector3 e2a = t1.c - t1.b;
			Vector3 e3a = t1.a - t1.c;

			Vector3 e1b = t2.b - t2.a;
			Vector3 e2b = t2.c - t2.b;
			Vector3 e3b = t2.a - t2.c;

			Vector3 normalA = e1a.Cross(e2a);
			normalA /= normalA.Length();
			Vector3 normalB = e1b.Cross(e2b);
			normalB /= normalB.Length();

			std::vector<Vector3> axes;

			axes.push_back(normalA);
			axes.push_back(normalB);

			Vector3 edgesA[] = { e1a, e2a, e3a };
			Vector3 edgesB[] = { e1b, e2b, e3b };

			for (int u = 0; u < 3; ++u)
			{
				for (int v = 0; v < 3; ++v)
				{
					Vector3 result = edgesA[u].Cross(edgesB[v]);
					result /= result.Length();
					axes.push_back(result);
				}
			}

			float minOverlap = FLT_MAX;
			Vector3 bestAxis(0, 0, 0);

			bool separation = false;
			for (int u = 0; u < axes.size(); u++)
			{
				Vector3 axis = axes[u];
				if ((axis.Length() * axis.Length()) < 1e-8f)
				{
					continue;
				}

				Projection p1 = ProjectOntoAxis(axis, t1);
				Projection p2 = ProjectOntoAxis(axis, t2);

				float overlap = std::min(p1.max, p2.max) - std::max(p1.min, p2.min);
				if (overlap < 0)
				{
					separation = true;
					break;
					//return IntersectData(false, Vector3(0,0,0), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
				}

				if (overlap < minOverlap)
				{
					minOverlap = overlap;
					bestAxis = axis;
					float centerDelta = axis.Dot(t2.a - t1.a);
					if (centerDelta < 0)
					{
						bestAxis = -bestAxis;
					}
				}
			}
			if (separation)
			{
				continue;
			}

			//Vector3 separationVector = bestAxis * minOverlap;

			collided = true;
			if (minOverlap < globalMinPenetration)
			{
				globalMinPenetration = minOverlap;
				globalBestAxis = bestAxis;
			}

			//return IntersectData(true, separationVector, Vector3(0,0,0), Vector3(0, 0, 0), Vector3(0, 0, 0));
				
		}
	}
	if (collided)
	{
		Vector3 separationVector = globalBestAxis * globalMinPenetration;
		return IntersectData(true,
			separationVector,
			Vector3(0, 0, 0),
			Vector3(0, 0, 0),
			Vector3(0, 0, 0));
	}

	return IntersectData(false,
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0));
}

IntersectData MeshCollider::IntersectSphere(BoundingSphere* other)
{
	for (int i = 0; i < triangles.size(); i++)
	{
		Vector3 pointOnTriangle = ClosestPointOnTriangle(other->GetTransformedCenter(), GetTransformedMeshTriangle(triangles[i]));
		float distSq = (other->GetTransformedCenter() - pointOnTriangle).Length();
		distSq *= distSq;
		if (distSq <= (other->GetRadius() * other->GetRadius())) 
		{
			Vector3 direction = (other->GetTransformedCenter() - pointOnTriangle) / (other->GetTransformedCenter() - pointOnTriangle).Length();
			float depth = other->GetRadius() - sqrt(distSq);
			Vector3 separationVector = -direction * depth;

			Vector3 collisionPoint = other->GetTransformedCenter() + (-direction * other->GetRadius());

			Vector3 collisionPointRadiusForFirstBody = collisionPoint - GetTransformedCenter();
			Vector3 collisionPointRadiusForSecondBody = collisionPoint - other->GetTransformedCenter();

			return IntersectData(true, 
								separationVector, 
								collisionPoint, 
								collisionPointRadiusForFirstBody, 
								collisionPointRadiusForSecondBody);
		}
		else
		{
			continue;
		}
	}

	return IntersectData(false,
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0));
}

IntersectData MeshCollider::IntersectOBB(OBB* other)
{
	bool collided = false;
	Vector3 separationVector;
	for (int i = 0; i < triangles.size(); i++)
	{
		IntersectData data = TriangleIntersectOBB(GetTransformedMeshTriangle(triangles[i]), other);
		if (data.GetDoesIntersect())
		{
			if (!collided)
			{
				separationVector = data.GetDirection();
			}
			else if (data.GetDirection().Length() < separationVector.Length())
			{
				separationVector = data.GetDirection();
			}
			collided = true;
		}
	}

	return IntersectData(collided, separationVector, Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
}

IntersectData MeshCollider::IntersectCapsule(CapsuleCollider* other)
{
	bool collided = false;
	float globalMinPenetration = FLT_MAX;
	Vector3 globalBestAxis;
	Vector3 bestPointOnTriangle;
	Vector3 bestPointOnCapsule;

	Vector3 capsuleStart = other->GetTransformedBase();
	Vector3 capsuleEnd = other->GetTransformedTip();
	float capsuleRadius = other->GetRadius();

	for (const MeshCollider::MeshTriangle& tri : triangles)
	{
		Vector3 closestA, closestB;
		float minDistSq = FLT_MAX;

		const int numSegments = 5;
		for (int i = 0; i <= numSegments; ++i)
		{
			float t = static_cast<float>(i) / numSegments;
			Vector3 pointOnCapsule = capsuleStart + (capsuleEnd - capsuleStart) * t;
			Vector3 pointOnTri = ClosestPointOnTriangle(pointOnCapsule, tri);
			float distSq = (pointOnTri - pointOnCapsule).Length() * (pointOnTri - pointOnCapsule).Length();

			if (distSq < minDistSq)
			{
				minDistSq = distSq;
				closestA = pointOnCapsule;
				closestB = pointOnTri;
			}
		}

		if (minDistSq <= capsuleRadius * capsuleRadius)
		{
			collided = true;
			float dist = sqrtf(minDistSq);
			float penetrationDepth = capsuleRadius - dist;

			Vector3 normal = closestA - closestB;
			normal /= normal.Length();
			if ((normal.Length() * normal.Length()) < 1e-6f)
			{
				normal = Vector3(0, 1, 0);
			}

			if (penetrationDepth < globalMinPenetration)
			{
				globalMinPenetration = penetrationDepth;
				globalBestAxis = normal;
				bestPointOnCapsule = closestA;
				bestPointOnTriangle = closestB;
			}
		}
	}

	if (collided)
	{
		Vector3 separationVector = globalBestAxis * globalMinPenetration;
		Vector3 collisionPoint = bestPointOnTriangle;

		return IntersectData(true,
			separationVector,
			Vector3(0, 0, 0),
			Vector3(0, 0, 0),
			Vector3(0, 0, 0));
	}

	return IntersectData(false,
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0),
		Vector3(0, 0, 0));
}

Vector3 MeshCollider::GetTransformedCenter()
{
	if (rigidbody != nullptr)
	{
		return Vector3( rigidbody->GetPosition().x,
						rigidbody->GetPosition().y,
						rigidbody->GetPosition().z );
	}

	return Vector3( pOwner->GetWorldPosition().x,
					pOwner->GetWorldPosition().y,
					pOwner->GetWorldPosition().z );
}

MeshCollider::MeshTriangle MeshCollider::GetTransformedMeshTriangle(MeshTriangle meshTriangle)
{
	if (rigidbody != nullptr)
	{
		DirectX::XMVECTOR a = DirectX::XMVectorSet(meshTriangle.a.x, meshTriangle.a.y, meshTriangle.a.z, 1.0f);
		DirectX::XMVECTOR b = DirectX::XMVectorSet(meshTriangle.b.x, meshTriangle.b.y, meshTriangle.b.z, 1.0f);
		DirectX::XMVECTOR c = DirectX::XMVectorSet(meshTriangle.c.x, meshTriangle.c.y, meshTriangle.c.z, 1.0f);

		a = DirectX::XMVector3Transform(a, rigidbody->GetBodyTransformationMatrix());
		b = DirectX::XMVector3Transform(b, rigidbody->GetBodyTransformationMatrix());
		c = DirectX::XMVector3Transform(c, rigidbody->GetBodyTransformationMatrix());

		Vector3 va(DirectX::XMVectorGetX(a), DirectX::XMVectorGetY(a), DirectX::XMVectorGetZ(a));
		Vector3 vb(DirectX::XMVectorGetX(b), DirectX::XMVectorGetY(b), DirectX::XMVectorGetZ(b));
		Vector3 vc(DirectX::XMVectorGetX(c), DirectX::XMVectorGetY(c), DirectX::XMVectorGetZ(c));

		MeshTriangle transformedTriangle(va, vb, vc);
		return transformedTriangle;
	}

	DirectX::XMVECTOR a = DirectX::XMVectorSet(meshTriangle.a.x, meshTriangle.a.y, meshTriangle.a.z, 1.0f);
	DirectX::XMVECTOR b = DirectX::XMVectorSet(meshTriangle.b.x, meshTriangle.b.y, meshTriangle.b.z, 1.0f);
	DirectX::XMVECTOR c = DirectX::XMVectorSet(meshTriangle.c.x, meshTriangle.c.y, meshTriangle.c.z, 1.0f);

	a = DirectX::XMVector3Transform(a, pOwner->GetWorldTransform());
	b = DirectX::XMVector3Transform(b, pOwner->GetWorldTransform());
	c = DirectX::XMVector3Transform(c, pOwner->GetWorldTransform());

	Vector3 va(DirectX::XMVectorGetX(a), DirectX::XMVectorGetY(a), DirectX::XMVectorGetZ(a));
	Vector3 vb(DirectX::XMVectorGetX(b), DirectX::XMVectorGetY(b), DirectX::XMVectorGetZ(b));
	Vector3 vc(DirectX::XMVectorGetX(c), DirectX::XMVectorGetY(c), DirectX::XMVectorGetZ(c));

	MeshTriangle transformedTriangle(va, vb, vc);
	return transformedTriangle;
}

IntersectData MeshCollider::TriangleIntersectOBB(MeshTriangle triangle, OBB* obb)
{
	float minPenetration = FLT_MAX;
	Vector3 bestAxis;

	std::vector<Vector3> axes;

	Vector3 edge1 = triangle.b - triangle.a;
	Vector3 edge2 = triangle.c - triangle.a;
	Vector3 triangleNormal = edge1.Cross(edge2);
	triangleNormal /= triangleNormal.Length();
	axes.push_back(triangleNormal);

	axes.push_back(obb->GetTransformedOrientation(obb->orientationX));
	axes.push_back(obb->GetTransformedOrientation(obb->orientationY));
	axes.push_back(obb->GetTransformedOrientation(obb->orientationZ));

	Vector3 triEdges[3] = { triangle.b - triangle.a,
							triangle.c - triangle.b,
							triangle.a - triangle.c};

	for (Vector3 edge : triEdges)
	{
		Vector3 crossAxis = edge.Cross(obb->GetTransformedOrientation(obb->orientationX));
		if (crossAxis.Dot(crossAxis) > 1e-6f)
		{
			crossAxis /= crossAxis.Length();
			axes.push_back(crossAxis);
		}

		crossAxis = edge.Cross(obb->GetTransformedOrientation(obb->orientationY));
		if (crossAxis.Dot(crossAxis) > 1e-6f)
		{
			crossAxis /= crossAxis.Length();
			axes.push_back(crossAxis);
		}

		crossAxis = edge.Cross(obb->GetTransformedOrientation(obb->orientationZ));
		if (crossAxis.Dot(crossAxis) > 1e-6f)
		{
			crossAxis /= crossAxis.Length();
			axes.push_back(crossAxis);
		}
	}

	Vector3 localVerts[3];
	for (int i = 0; i < 3; i++)
	{
		Vector3 v;
		if (i == 0)
		{
			v = triangle.a - obb->GetTransformedCenter();
		}
		if (i == 1)
		{
			v = triangle.b - obb->GetTransformedCenter();
		}
		if (i == 2)
		{
			v = triangle.c - obb->GetTransformedCenter();
		}
		localVerts[i] = Vector3(v.Dot(obb->GetTransformedOrientation(obb->orientationX)),
								v.Dot(obb->GetTransformedOrientation(obb->orientationY)),
								v.Dot(obb->GetTransformedOrientation(obb->orientationZ)));
	}

	Vector3 obbHalfSize(obb->GetTransformedSize().x * 0.5f,
						obb->GetTransformedSize().y * 0.5f,
						obb->GetTransformedSize().z * 0.5f);

	Vector3 aabbMin(-obbHalfSize.x,
					-obbHalfSize.y,
					-obbHalfSize.z);
	Vector3 aabbMax(obbHalfSize.x,
					obbHalfSize.y,
					obbHalfSize.z);

	for (Vector3 axis : axes)
	{
		float triMin = FLT_MAX;
		float triMax = -FLT_MAX;
		for (int i = 0; i < 3; i++)
		{
			float proj = localVerts[i].Dot(axis);
			triMin = std::min(triMin, proj);
			triMax = std::max(triMax, proj);
		}

		float boxCenter = 0.0f;

		float boxRadius =
			obbHalfSize.x * std::abs(axis.Dot(Vector3(1, 0, 0))) +
			obbHalfSize.y * std::abs(axis.Dot(Vector3(0, 1, 0))) +
			obbHalfSize.z * std::abs(axis.Dot(Vector3(0, 0, 1)));

		float boxMin = boxCenter - boxRadius;
		float boxMax = boxCenter + boxRadius;

		float overlap = std::min(triMax, boxMax) - std::max(triMin, boxMin);
		if (overlap < 0.0f)
		{
			return IntersectData(false, Vector3(0,0,0), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
		}

		
		float triLength = triMax - triMin;
		float boxLength = boxMax - boxMin;

		
		float projectedPenetration = triLength + boxLength - (std::max(triMax, boxMax) - std::min(triMin, boxMin));

		
		if (projectedPenetration < minPenetration - 1e-5f && projectedPenetration > 0.0f)
		{
			minPenetration = projectedPenetration;
			bestAxis = axis;
		}
	}

	Vector3 separationVector = bestAxis * minPenetration;

	return IntersectData(true, separationVector, Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
}

Vector3 MeshCollider::ClosestPointOnTriangle(Vector3 point, MeshCollider::MeshTriangle triangle)
{
	Vector3 ab = triangle.b - triangle.a;
	Vector3 ac = triangle.c - triangle.a;
	Vector3 ap = point - triangle.a;

	float d1 = ab.Dot(ap);
	float d2 = ac.Dot(ap);
	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		return triangle.a;
	}

	Vector3 bp = point - triangle.b;
	float d3 = ab.Dot(bp);
	float d4 = ac.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3)
	{
		return triangle.b;
	}

	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		return triangle.a + ab * v;
	}

	Vector3 cp = point - triangle.c;
	float d5 = ab.Dot(cp);
	float d6 = ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6)
	{
		return triangle.c;
	}

	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		return triangle.a + ac * w;
	}

	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return triangle.b + (triangle.c - triangle.b) * w;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return triangle.a + ab * v + ac * w;
}

Projection MeshCollider::ProjectOntoAxis(Vector3 axis, MeshCollider::MeshTriangle triangle)
{
	float p1 = axis.Dot(triangle.a);
	float p2 = axis.Dot(triangle.b);
	float p3 = axis.Dot(triangle.c);
	float min = MinFloat(p1, p2 ,p3);
	float max = MaxFloat(p1, p2, p3);

	Projection result;
	result.min = min;
	result.max = max;

	return result;
}

bool MeshCollider::OverlapOnAxis(MeshCollider::MeshTriangle t1, MeshCollider::MeshTriangle t2, Vector3 axis)
{
	if ((axis.Length() * axis.Length()) < 1e-8f)
	{
		return true;
	}

	Projection p1 = ProjectOntoAxis(axis, t1);
	Projection p2 = ProjectOntoAxis(axis, t2);

	return !(p1.max < p2.min || p2.max < p1.min);
}