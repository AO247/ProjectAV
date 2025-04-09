#pragma once
#include <SimpleMath.h>
#include "IntersectData.h"

using namespace DirectX::SimpleMath;

class AABB
{
public:
	AABB(Vector3& minExtents, Vector3& maxExtents) :
		minExtents(minExtents),
		maxExtents(maxExtents) {
	}

	IntersectData IntersectAABB(AABB& other);

	Vector3& GetMinExtents();
	Vector3& GetMaxExtents();

private:
	Vector3 minExtents;
	Vector3 maxExtents;
};