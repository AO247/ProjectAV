#include "Collider.h"
#include "BoundingSphere.h"
#include "AABB.h"

void Collider::Update(float dt)
{
}

void Collider::Draw(Graphics& gfx, DirectX::FXMMATRIX worldTransform)
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
	if (colliderType == SPHERE && other->GetColliderType() == AABB)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectAABB((AxisAligned::AABB*)other);
	}
	if (colliderType == AABB && other->GetColliderType() == SPHERE)
	{
		AxisAligned::AABB* self = (AxisAligned::AABB*)this;
		return self->IntersectBoundingSphere((BoundingSphere*)other);
	}
}

Collider::ColliderTypes Collider::GetColliderType()
{
	return colliderType;
}