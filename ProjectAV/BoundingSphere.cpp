#include "BoundingSphere.h"
#include <sstream>

IntersectData BoundingSphere::IntersectBoundingSphere(BoundingSphere& other)
{
	float radiusDistance = radius + other.radius;
	Vector3 direction = (other.GetTransformedCenter() - GetTransformedCenter());
	float centerDistance = direction.Length();
	direction /= centerDistance;

	float intersectionDistance = centerDistance - radiusDistance;
	Vector3 separationVector = direction * intersectionDistance;

	std::ostringstream ss;
	ss << direction.x;
	std::string s(ss.str());
	s += " TAKI\n";
	OutputDebugString(s.c_str());

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
	float testowy = rigidbody->GetPosition().x;
	return Vector3(rigidbody->GetPosition().x + center.x,
				   rigidbody->GetPosition().y + center.y,
				   rigidbody->GetPosition().z + center.z);
}
