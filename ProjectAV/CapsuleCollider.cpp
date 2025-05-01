#include "CapsuleCollider.h"

IntersectData CapsuleCollider::IntersectCapsule(CapsuleCollider* other)
{
	Vector3 selfNormal = (GetTransformedTip() - GetTransformedBase()) / ((GetTransformedTip() - GetTransformedBase()).Length());
	Vector3 selfLineEndOffset = selfNormal * radius;
	Vector3 selfA = GetTransformedBase() + selfLineEndOffset;
	Vector3 selfB = GetTransformedTip() - selfLineEndOffset;

	Vector3 otherNormal = (other->GetTransformedTip() - other->GetTransformedBase()) / ((other->GetTransformedTip() - other->GetTransformedBase()).Length());
	Vector3 otherLineEndOffset = otherNormal * other->radius;
	Vector3 otherA = other->GetTransformedBase() + otherLineEndOffset;
	Vector3 otherB = other->GetTransformedTip() - otherLineEndOffset;

	Vector3 v0 = otherA - selfA;
	Vector3 v1 = otherB - selfA;
	Vector3 v2 = otherA - selfB;
	Vector3 v3 = otherB - selfB;

	float d0 = v0.Dot(v0);
	float d1 = v1.Dot(v1);
	float d2 = v2.Dot(v2);
	float d3 = v3.Dot(v3);

	Vector3 bestA;
	if (d2 < d0 ||
		d2 < d1 ||
		d3 < d0 ||
		d3 < d1)
	{
		bestA = selfB;
	}
	else
	{
		bestA = selfA;
	}

	Vector3 bestB = ClosestPointOnLineSegment(otherA, otherB, bestA);

	bestA = ClosestPointOnLineSegment(selfA, selfB, bestB);

	Vector3 penetrationNormal = bestA - bestB;
	float len = penetrationNormal.Length();
	penetrationNormal /= len;
	float penetrationDepth = radius + other->radius - len;

	Vector3 separationVector = penetrationNormal * penetrationDepth;
	Vector3 collisionPoint = bestA + (penetrationNormal * radius);

	Vector3 collisionPointRadiusForFirstBody = collisionPoint - bestA;
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - bestB;

	return IntersectData(penetrationDepth > 0, separationVector, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
}

IntersectData CapsuleCollider::IntersectSphere(BoundingSphere* other)
{
	Vector3 selfNormal = (GetTransformedTip() - GetTransformedBase()) / ((GetTransformedTip() - GetTransformedBase()).Length());
	Vector3 selfLineEndOffset = selfNormal * radius;
	Vector3 selfA = GetTransformedBase() + selfLineEndOffset;
	Vector3 selfB = GetTransformedTip() - selfLineEndOffset;

	Vector3 bestA = ClosestPointOnLineSegment(selfA, selfB, other->GetTransformedCenter());

	Vector3 penetrationNormal = bestA - other->GetTransformedCenter();
	float len = penetrationNormal.Length();
	penetrationNormal /= len;
	float penetrationDepth = radius + other->GetRadius() - len;

	Vector3 separationVector = penetrationNormal * penetrationDepth;
	Vector3 collisionPoint = bestA + (penetrationNormal * radius);

	Vector3 collisionPointRadiusForFirstBody = collisionPoint - bestA;
	Vector3 collisionPointRadiusForSecondBody = collisionPoint - other->GetTransformedCenter();

	return IntersectData(penetrationDepth > 0, separationVector, collisionPoint, collisionPointRadiusForFirstBody, collisionPointRadiusForSecondBody);
}

IntersectData CapsuleCollider::IntersectOBB(OBB* other)
{
	Vector3 selfNormal = (GetTransformedTip() - GetTransformedBase()) / ((GetTransformedTip() - GetTransformedBase()).Length());
	Vector3 selfLineEndOffset = selfNormal * radius;
	Vector3 selfA = GetTransformedBase() + selfLineEndOffset;
	Vector3 selfB = GetTransformedTip() - selfLineEndOffset;

	Vector3 bestA = ClosestPointOnLineSegment(selfA, selfB, other->GetTransformedCenter());
	// mam tutaj fajne wartosci to mozna debugowac i recznie policzyc jak powinno byc
	Vector3 nearest = other->NearestPoint(bestA);
	Vector3 fromSphereCenterToNearestPoint = nearest - bestA;
	float dist = fromSphereCenterToNearestPoint.Length();

	Vector3 direction = fromSphereCenterToNearestPoint / dist;
	Vector3 separationVector = direction * (GetRadius() - dist);

	Vector3 collisionPoint = bestA + (direction * GetRadius());
	Vector3 r1 = collisionPoint - bestA;
	Vector3 r2 = collisionPoint - other->GetTransformedCenter();

	if (dist < GetRadius())
	{
		OutputDebugString("asdasdsa");
	}

	return IntersectData(dist < GetRadius(), -separationVector, collisionPoint, r1, r2);
}



Vector3 CapsuleCollider::GetTransformedBase()
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet(base.x, base.y, base.z, 1.0f);
	e = DirectX::XMVector4Transform(e, rigidbody->GetTransformationMatrixFromNode());
	return Vector3(DirectX::XMVectorGetX(e),
		DirectX::XMVectorGetY(e),
		DirectX::XMVectorGetZ(e));
}

Vector3 CapsuleCollider::GetTransformedTip()
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet(tip.x, tip.y, tip.z, 1.0f);
	e = DirectX::XMVector4Transform(e, rigidbody->GetTransformationMatrixFromNode());
	return Vector3(DirectX::XMVectorGetX(e),
		DirectX::XMVectorGetY(e),
		DirectX::XMVectorGetZ(e));
}

float CapsuleCollider::GetRadius()
{
	return radius;
}

Vector3 CapsuleCollider::ClosestPointOnLineSegment(Vector3 a, Vector3 b, Vector3 point)
{
	Vector3 ab = b - a;
	float t = ((point - a).Dot(ab)) / (ab.Dot(ab));
	Vector3 result = (a + (min(max(t, 0), 1) * ab));
	return result;
}
