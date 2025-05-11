#include "CapsuleCollider.h"
#include <algorithm>
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

	Vector3 nearest = other->NearestPoint(bestA);
	Vector3 fromSphereCenterToNearestPoint = nearest - bestA;
	float dist = fromSphereCenterToNearestPoint.Length();

	Vector3 direction = fromSphereCenterToNearestPoint / dist;
	Vector3 separationVector = direction * (GetRadius() - dist);

	Vector3 collisionPoint = bestA + (direction * GetRadius());
	Vector3 r1 = collisionPoint - bestA;
	Vector3 r2 = collisionPoint - other->GetTransformedCenter();

	return IntersectData(dist < GetRadius(), -separationVector, collisionPoint, r1, r2);
}

RaycastData CapsuleCollider::IntersectRay(Raycast* ray)
{
	Vector3 a = (GetTransformedTip() - GetTransformedBase()) / ((GetTransformedTip() - GetTransformedBase()).Length());
	Vector3 selfLineEndOffset = a * radius;
	Vector3 selfA = GetTransformedBase() + selfLineEndOffset;
	Vector3 selfB = GetTransformedTip() - selfLineEndOffset;
	Vector3 n = ray->direction;
	Vector3 b = selfA - ray->origin;
	float h = (selfB - selfA).Length();
	float r = radius;

	// ----- SPHERE TEST -----

	// Base sphere

	EndSphereIntersectionData baseSphereData = EndSphereIntersection(ray, false);
	if (baseSphereData.isIntersecting)
	{
		return RaycastData(this, baseSphereData.closestPoint);
	}

	// Tip sphere

	EndSphereIntersectionData tipSphereData = EndSphereIntersection(ray, true);
	if (tipSphereData.isIntersecting)
	{
		return RaycastData(this, tipSphereData.closestPoint);
	}

	// ----- CYLINDER TEST -----

	Vector3 nxa = n.Cross(a);
	// Ray is parallel to the axis, and there is no intersection, or the intersection is a line
	if (nxa.Length() == 0)
	{
		return RaycastData(nullptr, Vector3(0, 0, 0));
	}

	// Ray does not intersect the cylinder
	float bDotnxa = b.Dot(nxa);
	if ( ((nxa.Dot(nxa) * r * r) - (a.Dot(a) * bDotnxa * bDotnxa)) < 0 )
	{
		return RaycastData(nullptr, Vector3(0, 0, 0));
	}

	Vector3 bxa = b.Cross(a);

	float dotProduct = nxa.Dot(bxa);

	float root = sqrt( (nxa.Dot(nxa) * r * r) - (a.Dot(a) * bDotnxa * bDotnxa) );

	float denominator = nxa.Dot(nxa);

	float d1 = (dotProduct - root) / denominator;
	float d2 = (dotProduct + root) / denominator;

	// Both points are behind the ray origin
	if (d1 < 0 && d2 < 0)
	{
		return RaycastData(nullptr, Vector3(0, 0, 0));
	}

	// Keep the positive point in d1
	if (d1 < 0)
	{
		d1 = d2;
	}

	// Keep closer point in d1
	if (d1 > d2)
	{
		d1 = d2;
	}

	float t = a.Dot((n * d1) - b);

	// The point is inside the cylinder
	if (t >= 0 && t <= h)
	{
		Vector3 hitPoint = ray->origin + (n * d1);
		return RaycastData(this, hitPoint);
	}

	return RaycastData(nullptr, Vector3(0, 0, 0));
}

Vector3 CapsuleCollider::GetTransformedBase()
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet (base.x,
												base.y,
												base.z,
												1.0f);
	if (rigidbody != nullptr)
	{
		e = DirectX::XMVector4Transform(e, rigidbody->GetBodyTransformationMatrix());
		return Vector3(DirectX::XMVectorGetX(e),
			DirectX::XMVectorGetY(e),
			DirectX::XMVectorGetZ(e));
	}
	
	e = DirectX::XMVector4Transform(e, pOwner->GetWorldTransform());
	return Vector3(DirectX::XMVectorGetX(e),
		DirectX::XMVectorGetY(e),
		DirectX::XMVectorGetZ(e));
}

Vector3 CapsuleCollider::GetTransformedTip()
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet(tip.x,
		tip.y,
		tip.z,
		1.0f);
	if (rigidbody != nullptr)
	{
		e = DirectX::XMVector4Transform(e, rigidbody->GetBodyTransformationMatrix());
		return Vector3(DirectX::XMVectorGetX(e),
			DirectX::XMVectorGetY(e),
			DirectX::XMVectorGetZ(e));
	}

	e = DirectX::XMVector4Transform(e, pOwner->GetWorldTransform());
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
	Vector3 result = (a + (std::fmin(std::fmax(t, 0), 1) * ab));
	return result;
}

EndSphereIntersectionData CapsuleCollider::EndSphereIntersection(Raycast* ray, bool tipSphere)
{
	Vector3 a = (GetTransformedTip() - GetTransformedBase()) / ((GetTransformedTip() - GetTransformedBase()).Length());
	Vector3 selfLineEndOffset = a * radius;
	Vector3 selfA = GetTransformedBase() + selfLineEndOffset;
	Vector3 selfB = GetTransformedTip() - selfLineEndOffset;
	Vector3 n = ray->direction;
	Vector3 b = selfA - ray->origin;
	float h = (selfB - selfA).Length();
	float r = radius;

	Vector3 c;
	if (!tipSphere)
	{
		c = b;
	}
	else
	{
		c = b + (a * h);
	}

	float nDotC = n.Dot(c);
	float root = sqrt((nDotC * nDotC) + (r * r) - (c.Dot(c)));

	// No intersection
	if ( ((nDotC * nDotC) + (r * r) - (c.Dot(c))) < 0 )
	{
		return EndSphereIntersectionData(false, Vector3(0, 0, 0));
	}

	float d1 = nDotC - root;
	float d2 = nDotC + root;

	// Both points are behind the ray origin
	if (d1 < 0 && d2 < 0)
	{
		return EndSphereIntersectionData(false, Vector3(0, 0, 0));
	}

	// Keep the positive point in d1
	if (d1 < 0)
	{
		d1 = d2;
	}

	// Keep closer point in d1
	if (d1 > d2)
	{
		d1 = d2;
	}

	float t = a.Dot((n * d1) - b);
	
	if (!tipSphere)
	{
		if (t >= -r && t < 0)
		{
			Vector3 hitPoint = ray->origin + (n * d1);
			return EndSphereIntersectionData(true, hitPoint);
		}
	}
	else
	{
		if (t > h && t <= h + r)
		{
			Vector3 hitPoint = ray->origin + (n * d1);
			return EndSphereIntersectionData(true, hitPoint);
		}
	}

	return EndSphereIntersectionData(false, Vector3(0, 0, 0));

}
