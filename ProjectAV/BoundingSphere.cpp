#include "BoundingSphere.h"

IntersectData BoundingSphere::IntersectBoundingSphere(BoundingSphere& other)
{
	float radiusDistance = radius + other.radius;
	Vector3 direction = (other.center - center);
	float centerDistance = direction.Length();
	direction /= centerDistance;

	float intersectionDistance = centerDistance - radiusDistance;
	Vector3 separationVector = direction * intersectionDistance;

	return IntersectData(intersectionDistance < 0, separationVector);
}

Vector3& BoundingSphere::GetCenter()
{
	return center;
}

float BoundingSphere::GetRadius()
{
	return radius;
}
