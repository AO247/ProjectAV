#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class BoundingSphere : public Collider
{
public:
	BoundingSphere(Vector3& center, float radius) :
		center(center), 
		radius(radius),
		Collider(Collider::ColliderTypes::SPHERE) {}

	IntersectData IntersectBoundingSphere(BoundingSphere& other);

	Vector3& GetCenter();
	float GetRadius();

private:
	Vector3 center;
	float radius;
};

