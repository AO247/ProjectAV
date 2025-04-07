#include "Collider.h"
#include "BoundingSphere.h"

IntersectData Collider::Intersect(Collider* other)
{
	if (rigidbody != nullptr)
	{
		OutputDebugString("aaaaa");
	}
	if (colliderType == SPHERE && other->GetColliderType() == SPHERE)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectBoundingSphere((BoundingSphere&)other);
	}
}

Collider::ColliderTypes Collider::GetColliderType()
{
	return colliderType;
}