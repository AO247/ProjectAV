#include "Collider.h"
#include "BoundingSphere.h"
#include "AABB.h"

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
	if (colliderType == TYPE_OBB && other->GetColliderType() == TYPE_OBB)
	{
		OBB* self = (OBB*)this;
		return self->IntersectOBB((OBB*)other);
	}
	if (colliderType == SPHERE && other->GetColliderType() == TYPE_OBB)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectOBB((OBB*)other);
	}
	if (colliderType == TYPE_OBB && other->GetColliderType() == SPHERE)
	{
		OBB* self = (OBB*)this;
		return self->IntersectBoundingSphere((BoundingSphere*)other);
	}
}

Collider::ColliderTypes Collider::GetColliderType()
{
	return colliderType;
}