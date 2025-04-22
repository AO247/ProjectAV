#include "OBB.h"

IntersectData OBB::IntersectOBB(OBB* other)
{
	Vector3 axes[15] = { GetTransformedOrientation(orientationX), GetTransformedOrientation(orientationY), GetTransformedOrientation(orientationZ) ,
						 other->GetTransformedOrientation(other->orientationX), other->GetTransformedOrientation(other->orientationY), other->GetTransformedOrientation(other->orientationZ),
						 Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,0), 
						 Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,0), 
						 Vector3(0,0,0), Vector3(0,0,0), Vector3(0,0,0) };

	int index = 6;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 3; j < 6; j++)
		{
			axes[index] = axes[i].Cross(axes[j]);
			axes[index] /= axes[index].Length();
			index++;
		}
	}

	float minPenetration = FLT_MAX;
	Vector3 minAxis;

	for (int i = 0; i < 15; i++)
	{
		if (axes[i].Length() * axes[i].Length() < 1e-6f)
		{
			continue;
		}
		IntervalPair iA = this->GetInterval(axes[i]);
		IntervalPair iB = other->GetInterval(axes[i]);
		if (iB.imax < iA.imin || iA.imax < iB.imin)
		{
			return IntersectData(false, Vector3(0,0,0), Vector3(0, 0, 0), Vector3(0, 0, 0), Vector3(0, 0, 0));
		}
		float overlap = min(iA.imax, iB.imax) - max(iA.imin, iB.imin);
		if (overlap < minPenetration)
		{
			minPenetration = overlap;
			minAxis = axes[i];
		}
	}
	
	Vector3 direction = other->GetTransformedCenter() - this->GetTransformedCenter();
	if (minAxis.Dot(direction) < 0)
		minAxis = -minAxis;

	Vector3 mtv = minAxis * minPenetration;

	//Vector3 colPointA = this->GetClosestPoint(other->GetTransformedCenter());
	//Vector3 colPointB = other->GetClosestPoint(this->GetTransformedCenter());
	Vector3 colPointA = this->NearestPoint(other->GetTransformedCenter());
	Vector3 colPointB = other->NearestPoint(this->GetTransformedCenter());
	Vector3 collisionPoint = (colPointA + colPointB) * 0.5f;

	if (minPenetration > 0.8f && fabs(minAxis.Dot(GetTransformedOrientation(orientationY))) > 0.9f) {
		collisionPoint = this->GetTransformedCenter();
	}

	Vector3 r1 = collisionPoint - GetTransformedCenter();
	Vector3 r2 = collisionPoint - other->GetTransformedCenter();

	return IntersectData(true, -mtv, collisionPoint, r1, r2);

}

IntersectData OBB::IntersectBoundingSphere(BoundingSphere* other)
{
	Vector3 nearest = NearestPoint(other->GetTransformedCenter());
	Vector3 fromSphereCenterToNearestPoint = nearest - other->GetTransformedCenter();
	float dist = fromSphereCenterToNearestPoint.Length();
	
	Vector3 direction = fromSphereCenterToNearestPoint / dist;
	Vector3 separationVector = direction * (other->GetRadius() - dist);

	Vector3 collisionPoint = other->GetTransformedCenter() + (direction * other->GetRadius());
	Vector3 r1 = collisionPoint - GetTransformedCenter();
	Vector3 r2 = collisionPoint - other->GetTransformedCenter();

	return IntersectData(dist < other->GetRadius(), separationVector, collisionPoint, r1, r2);
}

Vector3 OBB::GetTransformedCenter()
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f);
	e = DirectX::XMVector3Transform(e, rigidbody->GetTransformationMatrixFromNode());
	return Vector3(DirectX::XMVectorGetX(e),
		DirectX::XMVectorGetY(e),
		DirectX::XMVectorGetZ(e));
}

Vector3 OBB::GetTransformedSize()
{
	DirectX::XMFLOAT3 scale = rigidbody->GetScaleFromNode();
	return Vector3(size.x * scale.x, size.y * scale.y, size.z * scale.z);
}

Vector3 OBB::GetTransformedVertex(Vector3 vertex)
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet(vertex.x, vertex.y, vertex.z, 1.0f);
	e = DirectX::XMVector4Transform(e, rigidbody->GetTransformationMatrixFromNode());
	return Vector3(DirectX::XMVectorGetX(e),
		DirectX::XMVectorGetY(e),
		DirectX::XMVectorGetZ(e));
}

Vector3 OBB::NearestPoint(Vector3 otherPoint)
{
	float rx = GetTransformedCenter().x;
	float ry = GetTransformedCenter().y;
	float rz = GetTransformedCenter().z;

	float dx = otherPoint.x - rx;
	float dy = otherPoint.y - ry;
	float dz = otherPoint.z - rz;

	Vector3 distance(dx, dy, dz);

	float sizes[3] = { GetTransformedSize().x / 2.0f, GetTransformedSize().y / 2.0f, GetTransformedSize().z / 2.0f};
	Vector3 axes[3] = { GetTransformedOrientation(orientationX), GetTransformedOrientation(orientationY), GetTransformedOrientation(orientationZ) };

	for (int i = 0; i < 3; i++)
	{
		Vector3 axis = axes[i];
		float dist = distance.Dot(axis);
		dist = std::clamp(dist, -sizes[i], sizes[i]);
		rx += axis.x * dist;
		ry += axis.y * dist;
		rz += axis.z * dist;
	}

	return Vector3(rx, ry, rz);
}

IntervalPair OBB::GetInterval(Vector3 axis)
{
	float xx = axis.x;
	float yy = axis.y;
	float zz = axis.z;
	Vector3 v(xx, yy, zz);

	float dot = v.Dot(GetTransformedVertex(vertices[0]));
	float imin = dot;
	float imax = dot;

	for (int i = 1; i < 8; i++)
	{
		dot = v.Dot(GetTransformedVertex(vertices[i]));
		imin = min(imin, dot);
		imax = max(imax, dot);
	}

	return IntervalPair(imin, imax);
}

bool OBB::ColOverlapAxis(OBB* first, OBB* second, Vector3 axis)
{
	IntervalPair a = first->GetInterval(axis);
	IntervalPair b = second->GetInterval(axis);
	return (b.imin <= a.imax) && (a.imin <= b.imax);
}

Vector3 OBB::GetClosestPoint(Vector3 point)
{
	Vector3 d = point - GetTransformedCenter();
	Vector3 closest = GetTransformedCenter();

	closest += GetTransformedOrientation(orientationX) * std::clamp(d.Dot(GetTransformedOrientation(orientationX)), -(GetTransformedSize().x / 2.0f), (GetTransformedSize().x / 2.0f));
	closest += GetTransformedOrientation(orientationY) * std::clamp(d.Dot(GetTransformedOrientation(orientationY)), -(GetTransformedSize().y / 2.0f), (GetTransformedSize().y / 2.0f));
	closest += GetTransformedOrientation(orientationZ) * std::clamp(d.Dot(GetTransformedOrientation(orientationZ)), -(GetTransformedSize().z / 2.0f), (GetTransformedSize().z / 2.0f));

	return closest;
}

Vector3 OBB::GetTransformedOrientation(Vector3 orientation)
{
	DirectX::XMVECTOR e = DirectX::XMVectorSet(orientation.x, orientation.y, orientation.z, 0.0f);
	Vector3 rot = rigidbody->GetRotationFromNode();
	DirectX::XMMATRIX r = DirectX::XMMatrixRotationRollPitchYaw(rot.y, rot.x, rot.z);
	e = DirectX::XMVector3Transform(e, r);
	Vector3 transformed = Vector3(DirectX::XMVectorGetX(e),
									DirectX::XMVectorGetY(e),
									DirectX::XMVectorGetZ(e));
	transformed /= transformed.Length();
	return transformed;
}
