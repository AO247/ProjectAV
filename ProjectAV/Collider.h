#pragma once
#include "IntersectData.h"
#include "Rigidbody.h"

class Rigidbody;

class Collider
{
public:
	enum ColliderTypes
	{
		SPHERE = 0,
		AABB = 1
	};

	Collider(ColliderTypes colliderType, Rigidbody* rigidbody) :
		colliderType(colliderType),
		rigidbody(rigidbody) {}

	IntersectData Intersect(Collider* other);

	ColliderTypes GetColliderType();

private:
	ColliderTypes colliderType;
protected:
	Rigidbody* rigidbody;
};