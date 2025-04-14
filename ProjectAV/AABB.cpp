#include "AABB.h"
#include "MiscMath.cpp"
#include "Box.h"

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

IntersectData AxisAligned::AABB::IntersectAABB(AABB* other)
{
	Vector3 distances1 = other->GetTransformedExtents(other->minExtents) - GetTransformedExtents(maxExtents);
	Vector3 distances2 = GetTransformedExtents(minExtents) - other->GetTransformedExtents(other->maxExtents);
	Vector3 distances = Vector3::Max(distances1, distances2);

	float maxDistance = MaxVectorElementValue(distances);

	float maxPenetration = distances.x;
	int axis = 0;

	if (distances.y > maxPenetration) {
		maxPenetration = distances.y;
		axis = 1;
	}
	if (distances.z > maxPenetration) {
		maxPenetration = distances.z;
		axis = 2;
	}

	Vector3 separation(0, 0, 0);
	if (maxPenetration < 0) {
		if (axis == 0) separation.x = maxPenetration;
		else if (axis == 1) separation.y = maxPenetration;
		else if (axis == 2) separation.z = maxPenetration;
	}

	return IntersectData(maxDistance < 0, -separation);
}

float mmyMax(float a, float b)
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

float mmyMin(float a, float b)
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

IntersectData AxisAligned::AABB::IntersectBoundingSphere(BoundingSphere* other)
{
	Vector3 pointClosestToSphereCenter(mmyMax(GetTransformedExtents(GetMinExtents()).x, mmyMin(other->GetTransformedCenter().x, GetTransformedExtents(GetMaxExtents()).x)),
									   mmyMax(GetTransformedExtents(GetMinExtents()).y, mmyMin(other->GetTransformedCenter().y, GetTransformedExtents(GetMaxExtents()).y)),
									   mmyMax(GetTransformedExtents(GetMinExtents()).z, mmyMin(other->GetTransformedCenter().z, GetTransformedExtents(GetMaxExtents()).z)));

	Vector3 direction = other->GetTransformedCenter() - pointClosestToSphereCenter;
	float penetrationDepth = other->GetRadius() - direction.Length();
	direction /= direction.Length();

	Vector3 separationVector = direction * penetrationDepth;

	return IntersectData(penetrationDepth > 0, -separationVector);
}

void AxisAligned::AABB::Draw(Graphics& gfx, DirectX::FXMMATRIX worldTransform)
{

}

Vector3& AxisAligned::AABB::GetMinExtents()
{
	return minExtents;
}

Vector3& AxisAligned::AABB::GetMaxExtents()
{
	return maxExtents;
}

Vector3 AxisAligned::AABB::GetTransformedExtents(Vector3 extents)
{
	return Vector3(rigidbody->GetPosition().x + extents.x,
				   rigidbody->GetPosition().y + extents.y, 
				   rigidbody->GetPosition().z + extents.z);
}
