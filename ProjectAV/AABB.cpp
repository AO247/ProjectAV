#include "AABB.h"
#include "MiscMath.cpp"

float MaxVectorElementValue(DirectX::SimpleMath::Vector3& vector)
{
	float max = vector.x;

	if (vector.y > max)
	{
		max = vector.y;
	}
	if (vector.z > max)
	{
		max = vector.z;
	}

	return max;
}

IntersectData AABB::IntersectAABB(AABB& other)
{
	Vector3 distances1 = other.minExtents - maxExtents;
	Vector3 distances2 = minExtents - other.maxExtents;
	Vector3 distances = Vector3::Max(distances1, distances2);

	float maxDistance = MaxVectorElementValue(distances);

	return IntersectData(maxDistance < 0, distances);
}

Vector3& AABB::GetMinExtents()
{
	return minExtents;
}

Vector3& AABB::GetMaxExtents()
{
	return maxExtents;
}