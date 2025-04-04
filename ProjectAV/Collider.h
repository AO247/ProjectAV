#pragma once
#include "IntersectData.h"

class Collider
{
public:
	enum ColliderTypes
	{
		SPHERE = 0,
		AABB = 1
	};

	Collider(ColliderTypes colliderType) :
		colliderType(colliderType) {}

	IntersectData Intersect(Collider& other);

	ColliderTypes GetColliderType();

private:
	ColliderTypes colliderType;
};

