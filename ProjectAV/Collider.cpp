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
	if (colliderType == AABB && other->GetColliderType() == AABB)
	{
		AxisAligned::AABB* self = (AxisAligned::AABB*)this;
		return self->IntersectAABB((AxisAligned::AABB*)other);
	}
}

Collider::ColliderTypes Collider::GetColliderType()
{
	return colliderType;
}