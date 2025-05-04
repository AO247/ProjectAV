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

IntersectData BoundingSphere::IntersectCapsule(CapsuleCollider* other)
{
	Vector3 selfNormal = (other->GetTransformedTip() - other->GetTransformedBase()) / ((other->GetTransformedTip() - other->GetTransformedBase()).Length());
	Vector3 selfLineEndOffset = selfNormal * other->GetRadius();
	Vector3 selfA = other->GetTransformedBase() + selfLineEndOffset;
	Vector3 selfB = other->GetTransformedTip() - selfLineEndOffset;

	Vector3 bestA = other->ClosestPointOnLineSegment(selfA, selfB, GetTransformedCenter());

	Vector3 penetrationNormal = bestA - GetTransformedCenter();
	float len = penetrationNormal.Length();
	penetrationNormal /= len;
	float penetrationDepth = radius + other->GetRadius() - len;

	Vector3 separationVector = penetrationNormal * penetrationDepth;
	Vector3 collisionPoint = bestA + (penetrationNormal * other->GetRadius());

	Vector3 collisionPointRadiusForFirstBody = collisionPoint - GetTransformedCenter();
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - bestA;

	return IntersectData(penetrationDepth > 0, -separationVector, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
}

RaycastData BoundingSphere::IntersectRay(Raycast* ray)
{
	Vector3 L = GetTransformedCenter() - ray->origin;
	float tca = L.Dot(ray->direction);
	if (tca < 0)
	{
		return RaycastData(nullptr, Vector3(0, 0, 0));
	}

	float d = sqrt((L.Length() * L.Length()) - (tca * tca));
	if (d > radius)
	{
		return RaycastData(nullptr, Vector3(0, 0, 0));
	}

	float thc = sqrt((radius*radius) - (d*d));

	float t0 = tca - thc;
	float t1 = tca + thc;

	if (t0 < 0)
	{
		t0 = t1;
	}

	return RaycastData(this, (ray->origin + (t0 * ray->direction)));
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
	if (rigidbody != nullptr)
	{
		return Vector3(rigidbody->GetPosition().x + center.x,
			rigidbody->GetPosition().y + center.y,
			rigidbody->GetPosition().z + center.z);
	}

	return Vector3 (pOwner->GetWorldPosition().x + center.x,
					pOwner->GetWorldPosition().y + center.y,
					pOwner->GetWorldPosition().z + center.z);
}
