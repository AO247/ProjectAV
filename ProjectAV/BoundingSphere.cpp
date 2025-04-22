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

	Vector3 collisionPoint = GetTransformedCenter() + (direction * GetRadius());

	Vector3 collisionPointRadiusForFirstBody = collisionPoint - GetTransformedCenter();
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - other->GetTransformedCenter();

	return IntersectData(intersectionDistance < 0, separationVector, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
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

	Vector3 collisionPoint = GetTransformedCenter() - (direction * GetRadius());

	Vector3 collisionPointRadiusForFirstBody = collisionPoint - GetTransformedCenter();
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - other->GetCenter();

	return IntersectData(penetrationDepth > 0, -separationVector, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
}

IntersectData BoundingSphere::IntersectOBB(OBB* other)
{
	Vector3 nearest = other->NearestPoint(GetTransformedCenter());
	Vector3 fromSphereCenterToNearestPoint = nearest - GetTransformedCenter();
	float dist = fromSphereCenterToNearestPoint.Length();

	Vector3 direction = fromSphereCenterToNearestPoint / dist;
	Vector3 separationVector = direction * (GetRadius() - dist);

	Vector3 collisionPoint = GetTransformedCenter() + (direction * GetRadius());
	Vector3 r1 = collisionPoint - GetTransformedCenter();
	Vector3 r2 = collisionPoint - other->GetTransformedCenter();

	return IntersectData(dist < GetRadius(), -separationVector, collisionPoint, r1, r2);
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
