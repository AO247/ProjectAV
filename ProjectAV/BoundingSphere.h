#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class BoundingSphere : public Collider
{
public:
	BoundingSphere(Node* owner, Vector3 center, float radius, Rigidbody* rigidbody) :
		center(center),
		radius(radius),
		Collider(owner, Collider::ColliderTypes::SPHERE, rigidbody) {
	}

	IntersectData IntersectBoundingSphere(BoundingSphere* other);

	Vector3& GetCenter();
	float GetRadius();
	Vector3 GetTransformedCenter();

private:
	Vector3 center;
	float radius;
};