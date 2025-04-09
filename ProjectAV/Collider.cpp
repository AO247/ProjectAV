#include "Collider.h"
#include "BoundingSphere.h"

void Collider::Update(float dt)
{
}

IntersectData Collider::Intersect(Collider* other)
{
	if (colliderType == SPHERE && other->GetColliderType() == SPHERE)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectBoundingSphere((BoundingSphere*)other);
	}
}

Collider::ColliderTypes Collider::GetColliderType()
{
	return colliderType;
}