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
			Triangle t1 = triangles[i];
			Triangle t2 = other->triangles[j];

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

				float overlap = min(p1.max, p2.max) - max(p1.min, p2.min);
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
		Vector3 pointOnTriangle = ClosestPointOnTriangle(other->GetTransformedCenter(), triangles[i]);
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
	float globalMinPenetration = FLT_MAX;
	Vector3 globalBestAxis;
	for (int i = 0; i < triangles.size(); i++)
	{
		Triangle tri = triangles[i];
		Vector3 boxCenter = other->GetTransformedCenter();
		Vector3 boxAxis[3] = { other->GetTransformedOrientation(other->orientationX),
								other->GetTransformedOrientation(other->orientationY),
								other->GetTransformedOrientation(other->orientationZ) };
		float halfExtents[3] = { other->GetTransformedSize().x / 2.0f,
								other->GetTransformedSize().y / 2.0f,
								other->GetTransformedSize().z / 2.0f };

		Vector3 v0 = tri.a - boxCenter;
		Vector3 v1 = tri.b - boxCenter;
		Vector3 v2 = tri.c - boxCenter;

		Vector3 triVerts[3] = { v0, v1, v2 };
		Vector3 triEdges[3] = { v1 - v0, v2 - v1, v0 - v2 };

		bool separation = false;

		float minPenetration = FLT_MAX;
		Vector3 bestAxis;

		for (int u = 0; u < 3; u++)
		{
			Vector3 axis = boxAxis[u];
			float r = halfExtents[u];
			float p0 = v0.Dot(axis);
			float p1 = v1.Dot(axis);
			float p2 = v2.Dot(axis);

			float minP = MinFloat(p0, p1, p2);
			float maxP = MaxFloat(p0, p1, p2);

			if (minP > r || maxP < -r)
			{
				separation = true;
				break;
			}

			float overlap = min(maxP, r) - max(minP, -r);
			if (overlap < minPenetration)
			{
				minPenetration = overlap;
				bestAxis = axis;
				Vector3 centerTri = (v0 + v1 + v2) / 3.0f;
				if (centerTri.Dot(bestAxis) < 0)
				{
					bestAxis = -bestAxis;
				}
			}
		}
		if (separation)
		{
			continue;
		}

		Vector3 triNormal = triEdges[0].Cross(triEdges[1]);
		triNormal /= triNormal.Length();
		float r = 0;
		for (int u = 0; u < 3; u++)
		{
			r += halfExtents[u] * abs(triNormal.Dot(boxAxis[u]));
		}

		float p = abs(v0.Dot(triNormal));
		if (p > r)
		{
			continue;
		}
		float overlap = r - p;
		if (overlap < minPenetration)
		{
			minPenetration = overlap;
			bestAxis = triNormal / triNormal.Length();
			Vector3 centerTri = (v0 + v1 + v2) / 3.0f;
			if (centerTri.Dot(bestAxis) < 0)
			{
				bestAxis = -bestAxis;
			}
		}

		for (int u = 0; u < 3; u++)
		{
			for (int v = 0; v < 3; v++)
			{
				Vector3 axis = triEdges[u].Cross(boxAxis[v]);
				if ((axis.Length() * axis.Length()) < 1e-6f)
				{
					continue;
				}

				axis /= axis.Length();

				float minT = FLT_MAX;
				float maxT = -minT;

				for (int w = 0; w < 3; w++)
				{
					float proj = triVerts[w].Dot(axis);
					minT = min(minT, proj);
					maxT = max(maxT, proj);
				}

				float rBox = 0;
				for (int w = 0; w < 3; w++)
				{
					rBox += halfExtents[w] * abs(axis.Dot(boxAxis[w]));
				}

				if (minT > rBox || maxT < -rBox)
				{
					separation = true;
					break;
				}

				float overlap = min(maxT, rBox) - max(minT, -rBox);
				if (overlap < minPenetration)
				{
					minPenetration = overlap;
					bestAxis = axis;
					Vector3 centerTri = (v0 + v1 + v2) / 3.0f;
					if (centerTri.Dot(bestAxis) < 0)
					{
						bestAxis = -bestAxis;
					}
				}
			}
			if (separation)
			{
				break;
			}
		}
		if (separation)
		{
			continue;
		}

		collided = true;
		if (minPenetration < globalMinPenetration)
		{
			globalMinPenetration = minPenetration;
			globalBestAxis = bestAxis;
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

	for (const Triangle& tri : triangles)
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

Vector3 MeshCollider::ClosestPointOnTriangle(Vector3 point, Triangle triangle)
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

Projection MeshCollider::ProjectOntoAxis(Vector3 axis, Triangle triangle)
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

bool MeshCollider::OverlapOnAxis(Triangle t1, Triangle t2, Vector3 axis)
{
	if ((axis.Length() * axis.Length()) < 1e-8f)
	{
		return true;
	}

	Projection p1 = ProjectOntoAxis(axis, t1);
	Projection p2 = ProjectOntoAxis(axis, t2);

	return !(p1.max < p2.min || p2.max < p1.min);
}