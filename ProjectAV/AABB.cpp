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

	Vector3 collisionPoint;
	if (axis == 0)
	{
		if (distances.x > 0)
		{
			collisionPoint.x = GetTransformedExtents(maxExtents).x;
			collisionPoint.y = GetTransformedExtents(maxExtents - (GetEdgeSizes() / 2.0f)).y;
			collisionPoint.z = GetTransformedExtents(maxExtents - (GetEdgeSizes() / 2.0f)).z;
		}
		else
		{
			collisionPoint.x = GetTransformedExtents(minExtents).x;
			collisionPoint.y = GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).y;
			collisionPoint.z = GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).z;
		}
	}
	if (axis == 1)
	{
		if (distances.y > 0)
		{
			collisionPoint.x = GetTransformedExtents(maxExtents - (GetEdgeSizes() / 2.0f)).x;
			collisionPoint.y = GetTransformedExtents(maxExtents).y;
			collisionPoint.z = GetTransformedExtents(maxExtents - (GetEdgeSizes() / 2.0f)).z;
		}
		else
		{
			collisionPoint.x = GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).x;
			collisionPoint.y = GetTransformedExtents(minExtents).y;
			collisionPoint.z = GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).z;
		}
	}
	if (axis == 2)
	{
		if (distances.z > 0)
		{
			collisionPoint.x = GetTransformedExtents(maxExtents - (GetEdgeSizes() / 2.0f)).x;
			collisionPoint.y = GetTransformedExtents(maxExtents - (GetEdgeSizes() / 2.0f)).y;
			collisionPoint.z = GetTransformedExtents(maxExtents).z;
		}
		else
		{
			collisionPoint.x = GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).x;
			collisionPoint.y = GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).y ;
			collisionPoint.z = GetTransformedExtents(minExtents).z;
		}
	}

	Vector3 ccc = GetCenter();
	Vector3 collisionPointRadiusForFirstBody = collisionPoint - GetCenter();
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - other->GetCenter();

	if (maxDistance < 0)
	{
		OutputDebugString("sztop");
	}

	return IntersectData(maxDistance < 0, -separation, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
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
	
	Vector3 collisionPoint = other->GetTransformedCenter() - (direction * other->GetRadius());

	Vector3 collisionPointRadiusForFirstBody = collisionPoint - GetCenter();
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - other->GetTransformedCenter();

	return IntersectData(penetrationDepth > 0, -separationVector, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
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
	/*Vector3 actualExtents(rigidbody->GetPosition().x + extents.x,
						  rigidbody->GetPosition().y + extents.y,
						  rigidbody->GetPosition().z + extents.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rigidbody->GetRotation().y, rigidbody->GetRotation().x, rigidbody->GetRotation().z);
	DirectX::XMVECTOR e = DirectX::XMVectorSet(actualExtents.x, actualExtents.y, actualExtents.z, 1.0f);
	e = DirectX::XMVector3Transform(e, rotationMatrix);
	return Vector3(DirectX::XMVectorGetX(e),
				   DirectX::XMVectorGetY(e),
				   DirectX::XMVectorGetZ(e));*/
	//return actualExtents;

	DirectX::XMVECTOR e = DirectX::XMVectorSet(extents.x, extents.y, extents.z, 1.0f);
	e = DirectX::XMVector3Transform(e, rigidbody->GetTransformationMatrixFromNode());
	return Vector3(DirectX::XMVectorGetX(e),
				   DirectX::XMVectorGetY(e),
				   DirectX::XMVectorGetZ(e));
}

Vector3 AxisAligned::AABB::GetEdgeSizes()
{
	return Vector3(maxExtents.x - minExtents.x,
				   maxExtents.y - minExtents.y,
				   maxExtents.z - minExtents.z);
}

Vector3 AxisAligned::AABB::GetCenter()
{

	return Vector3(GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).x,
				   GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).y,
				   GetTransformedExtents(minExtents + (GetEdgeSizes() / 2.0f)).z);
}

Rigidbody* AxisAligned::AABB::GetRigidbody()
{
	return rigidbody;
}
