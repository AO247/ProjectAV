#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class BoundingSphere : public Collider
{
public:
	BoundingSphere(Vector3 center, float radius, std::shared_ptr<Rigidbody> rigidbody) :
		center(center),
		radius(radius),
		Collider(Collider::ColliderTypes::SPHERE, rigidbody) {
	}

	IntersectData IntersectBoundingSphere(BoundingSphere& other);

	Vector3& GetCenter();
	float GetRadius();
	Vector3 GetTransformedCenter();

private:
	Vector3 center;
	float radius;
};