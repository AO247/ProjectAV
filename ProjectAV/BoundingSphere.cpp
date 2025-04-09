#include "BoundingSphere.h"
#include <sstream>

IntersectData BoundingSphere::IntersectBoundingSphere(BoundingSphere* other)
{
	float radiusDistance = radius + other->radius;
	Vector3 direction = (other->GetTransformedCenter() - GetTransformedCenter());
	//Vector3 v1 = other->GetTransformedCenter();
	//Vector3 v2(10.0f, 0.0f, 0.0f);
	//Vector3 r = v1 + v2;
	//std::ostringstream ss2;
	//ss2 << r.x;
	//std::string s2(ss2.str());
	//s2 += " DIR\n";
	//OutputDebugString(s2.c_str());
	float centerDistance = direction.Length();
	direction /= centerDistance;

	float intersectionDistance = centerDistance - radiusDistance;
	Vector3 separationVector = direction * intersectionDistance;

	std::ostringstream ss2;
	ss2 << intersectionDistance;
	std::string s2(ss2.str());
	s2 += " GLEBOKOSC PRZECIECIA\n";
	OutputDebugString(s2.c_str());

	std::ostringstream ss3;
	ss3 << separationVector.x;
	std::string s3(ss3.str());
	s3 += " X SEPARACJI\n";
	OutputDebugString(s3.c_str());

	/*std::ostringstream ss;
	ss << other->GetTransformedCenter().x;
	std::string s(ss.str());
	s += " TAKI\n";
	OutputDebugString(s.c_str());
	std::ostringstream ss1;
	ss1 << GetTransformedCenter().x;
	std::string s1(ss1.str());
	s1 += " TAKI NASTEPNY\n";
	OutputDebugString(s1.c_str());*/

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
	return Vector3(rigidbody->GetPosition().x + center.x,
				   rigidbody->GetPosition().y + center.y,
				   rigidbody->GetPosition().z + center.z);
}
