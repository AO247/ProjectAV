#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"
#include "Collider.h"
#include "BoundingSphere.h"

using namespace DirectX::SimpleMath;

class BoundingSphere;

namespace AxisAligned
{
	class AABB : public Collider
	{
	public:
		AABB(Node* owner, Vector3 minExtents, Vector3 maxExtents, Rigidbody* rigidbody) :
			minExtents(minExtents),
			maxExtents(maxExtents),
			Collider(owner, Collider::ColliderTypes::AABB, rigidbody)
		{
		}

		void Draw(Graphics& gfx, DirectX::FXMMATRIX worldTransform);

		IntersectData IntersectAABB(AABB* other);
		IntersectData IntersectBoundingSphere(BoundingSphere* other);

		Vector3& GetMinExtents();
		Vector3& GetMaxExtents();
		Vector3 GetTransformedExtents(Vector3 extents);
	private:
		Vector3 minExtents;
		Vector3 maxExtents;
	};
}
