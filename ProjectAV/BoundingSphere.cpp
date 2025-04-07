#include "BoundingSphere.h"

IntersectData BoundingSphere::IntersectBoundingSphere(BoundingSphere& other)
{
	float radiusDistance = radius + other.radius;
	Vector3 direction = (other.GetTransformedCenter() - GetTransformedCenter());
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

Vector3 BoundingSphere::GetTransformedCenter()
{
	if (rigidbody != nullptr)
	{
		OutputDebugString("essa");
	}
	return Vector3(rigidbody->GetPosition().x + center.x,
				   rigidbody->GetPosition().y + center.y,
				   rigidbody->GetPosition().z + center.z);
}
