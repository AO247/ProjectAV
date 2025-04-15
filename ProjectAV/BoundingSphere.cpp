#include "BoundingSphere.h"
#include <sstream>
#include <algorithm>

IntersectData BoundingSphere::IntersectBoundingSphere(BoundingSphere* other)
{
	float radiusDistance = radius + other->radius;
	Vector3 direction = (other->GetTransformedCenter() - GetTransformedCenter());
	float centerDistance = direction.Length();
	direction /= centerDistance;

	float intersectionDistance = centerDistance - radiusDistance;
	Vector3 separationVector = direction * intersectionDistance;

	return IntersectData(intersectionDistance < 0, separationVector);
}

float myMax(float a, float b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

float myMin(float a, float b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

IntersectData BoundingSphere::IntersectAABB(AxisAligned::AABB* other)
{
	Vector3 pointClosestToSphereCenter(myMax(other->GetTransformedExtents(other->GetMinExtents()).x, myMin(GetTransformedCenter().x, other->GetTransformedExtents(other->GetMaxExtents()).x)),
									   myMax(other->GetTransformedExtents(other->GetMinExtents()).y, myMin(GetTransformedCenter().y, other->GetTransformedExtents(other->GetMaxExtents()).y)),
									   myMax(other->GetTransformedExtents(other->GetMinExtents()).z, myMin(GetTransformedCenter().z, other->GetTransformedExtents(other->GetMaxExtents()).z)));

	Vector3 direction = pointClosestToSphereCenter - GetTransformedCenter();
	float penetrationDepth = radius - direction.Length();
	direction /= direction.Length();

	Vector3 separationVector = direction * penetrationDepth;

	return IntersectData(penetrationDepth > 0, -separationVector);
}

Vector3& BoundingSphere::GetCenter()
{
	return center;
}

float BoundingSphere::GetRadius()
{
	return radius;
}

Vector3 BoundingSphere::GetTransformedCenter()
{
	return Vector3(rigidbody->GetPosition().x + center.x,
				   rigidbody->GetPosition().y + center.y,
				   rigidbody->GetPosition().z + center.z);
}
