#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class IntersectData
{
public:
	IntersectData(bool doesIntersect, Vector3& direction) :
		doesIntersect(doesIntersect),
		direction(direction) {}

	bool GetDoesIntersect();
	float GetDistance();
	Vector3& GetDirection();
private:
	bool doesIntersect;
	Vector3 direction;
};

