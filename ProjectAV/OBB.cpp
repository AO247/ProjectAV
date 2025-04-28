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

	//int firstCollidingSurfaceIndex = -1;
	//int secondCollidingSurfaceIndex = -1;

	IntervalPair projectionDataForFirstFace;
	IntervalPair projectionDataForSecondFace;

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
		/*if (overlap == minPenetration && i < 6 && i > 2)
		{
			secondCollidingSurfaceIndex = i;
		}*/
		if (overlap < minPenetration)
		{
			minPenetration = overlap;
			minAxis = axes[i];
			projectionDataForFirstFace = iA;
			projectionDataForSecondFace = iB;
			//if (i < 3)
			//{
			//	firstCollidingSurfaceIndex = i;
			//}
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

	// Collision type is face to face

	/*if (ProjectAndCheckOverlapping(0, projectionDataForFirstFace, projectionDataForSecondFace) ||
		ProjectAndCheckOverlapping(1, projectionDataForFirstFace, projectionDataForSecondFace) || 
		ProjectAndCheckOverlapping(2, projectionDataForFirstFace, projectionDataForSecondFace))
	{
		r1 = Vector3(0, 0, 0);
		r2 = Vector3(0, 0, 0);
	}*/

	// OX OZ Projection


	/*if (firstCollidingSurfaceIndex != -1 && secondCollidingSurfaceIndex != -1)
	{
		Vector3 firstSurfaceVertices[4];
		Vector3 secondSurfaceVertices[4];

		if (firstCollidingSurfaceIndex == 0)
		{

		}


		r1 = NearestPoint((GetTransformedCenter() + axes[firstCollidingSurfaceIndex])) - GetTransformedCenter();
		r2 = other->NearestPoint((other->GetTransformedCenter() + axes[secondCollidingSurfaceIndex])) - other->GetTransformedCenter();
	}*/

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
	IntervalPair result;

	float xx = axis.x;
	float yy = axis.y;
	float zz = axis.z;
	Vector3 v(xx, yy, zz);

	float dot = v.Dot(GetTransformedVertex(vertices[0]));
	float imin = dot;
	float imax = dot;

	int closestVertexIndex = 0;

	for (int i = 1; i < 8; i++)
	{
		dot = v.Dot(GetTransformedVertex(vertices[i]));
		imin = min(imin, dot);
		if (imax == dot)
		{
			//result.verticesClosestToTheAxis[closestVertexIndex] = GetTransformedVertex(vertices[i]);
			closestVertexIndex++;
		}
		imax = max(imax, dot);
	}

	result.imin = imin;
	result.imax = imax;

	return result;
}

bool OBB::ColOverlapAxis(OBB* first, OBB* second, Vector3 axis)
{
	IntervalPair a = first->GetInterval(axis);
	IntervalPair b = second->GetInterval(axis);
	return (b.imin <= a.imax) && (a.imin <= b.imax);
}

// surface:
// 0 - OX OZ
// 1 - OX OY
// 2 - OZ OY
/*bool OBB::ProjectAndCheckOverlapping(int surface, IntervalPair data1, IntervalPair data2)
{
	// Projection stage

	Vector3 surfaceNormal(0.0f, 1.0f, 0.0f);
	Vector3 surfacePoint(0.0f, 0.0f, 0.0f);

	switch (surface)
	{
	case 0:
	{
		surfaceNormal = Vector3(0.0f, 1.0f, 0.0f);

		float a = surfaceNormal.x;
		float b = surfaceNormal.y;
		float c = surfaceNormal.z;

		float d = surfacePoint.x;
		float e = surfacePoint.y;
		float f = surfacePoint.z;

		for (int i = 0; i < 4; i++)
		{
			float x = data1.verticesClosestToTheAxis[i].x;
			float y = data1.verticesClosestToTheAxis[i].y;
			float z = data1.verticesClosestToTheAxis[i].z;

			float t = ((a * d) - (a * x) + (b * e) - (b * y) + (c * f) - (c * z)) / ((a * a) + (b * b) + (c * c));

			data1.verticesClosestToTheAxis[i].x = x + t * a;
			data1.verticesClosestToTheAxis[i].y = y + t * b;
			data1.verticesClosestToTheAxis[i].z = z + t * c;
		}
		for (int i = 0; i < 4; i++)
		{
			float x = data2.verticesClosestToTheAxis[i].x;
			float y = data2.verticesClosestToTheAxis[i].y;
			float z = data2.verticesClosestToTheAxis[i].z;

			float t = ((a * d) - (a * x) + (b * e) - (b * y) + (c * f) - (c * z)) / ((a * a) + (b * b) + (c * c));

			data2.verticesClosestToTheAxis[i].x = x + t * a;
			data2.verticesClosestToTheAxis[i].y = y + t * b;
			data2.verticesClosestToTheAxis[i].z = z + t * c;
		}
		break;
	}
	case 1:
	{
		//Vector3 surfaceNormal(0.0f, 0.0f, 1.0f);
		//Vector3 surfacePoint(0.0f, 0.0f, 0.0f);

		surfaceNormal = Vector3(0.0f, 0.0f, 1.0f);

		float a = surfaceNormal.x;
		float b = surfaceNormal.y;
		float c = surfaceNormal.z;

		float d = surfacePoint.x;
		float e = surfacePoint.y;
		float f = surfacePoint.z;

		for (int i = 0; i < 4; i++)
		{
			float x = data1.verticesClosestToTheAxis[i].x;
			float y = data1.verticesClosestToTheAxis[i].y;
			float z = data1.verticesClosestToTheAxis[i].z;

			float t = ((a * d) - (a * x) + (b * e) - (b * y) + (c * f) - (c * z)) / ((a * a) + (b * b) + (c * c));

			data1.verticesClosestToTheAxis[i].x = x + t * a;
			data1.verticesClosestToTheAxis[i].y = y + t * b;
			data1.verticesClosestToTheAxis[i].z = z + t * c;
		}
		for (int i = 0; i < 4; i++)
		{
			float x = data2.verticesClosestToTheAxis[i].x;
			float y = data2.verticesClosestToTheAxis[i].y;
			float z = data2.verticesClosestToTheAxis[i].z;

			float t = ((a * d) - (a * x) + (b * e) - (b * y) + (c * f) - (c * z)) / ((a * a) + (b * b) + (c * c));

			data2.verticesClosestToTheAxis[i].x = x + t * a;
			data2.verticesClosestToTheAxis[i].y = y + t * b;
			data2.verticesClosestToTheAxis[i].z = z + t * c;
		}
		break;
	}
	case 2:
	{
		//Vector3 surfaceNormal(1.0f, 0.0f, 0.0f);
		//Vector3 surfacePoint(0.0f, 0.0f, 0.0f);

		surfaceNormal = Vector3(1.0f, 0.0f, 0.0f);

		float a = surfaceNormal.x;
		float b = surfaceNormal.y;
		float c = surfaceNormal.z;

		float d = surfacePoint.x;
		float e = surfacePoint.y;
		float f = surfacePoint.z;

		for (int i = 0; i < 4; i++)
		{
			float x = data1.verticesClosestToTheAxis[i].x;
			float y = data1.verticesClosestToTheAxis[i].y;
			float z = data1.verticesClosestToTheAxis[i].z;

			float t = ((a * d) - (a * x) + (b * e) - (b * y) + (c * f) - (c * z)) / ((a * a) + (b * b) + (c * c));

			data1.verticesClosestToTheAxis[i].x = x + t * a;
			data1.verticesClosestToTheAxis[i].y = y + t * b;
			data1.verticesClosestToTheAxis[i].z = z + t * c;
		}
		for (int i = 0; i < 4; i++)
		{
			float x = data2.verticesClosestToTheAxis[i].x;
			float y = data2.verticesClosestToTheAxis[i].y;
			float z = data2.verticesClosestToTheAxis[i].z;

			float t = ((a * d) - (a * x) + (b * e) - (b * y) + (c * f) - (c * z)) / ((a * a) + (b * b) + (c * c));

			data2.verticesClosestToTheAxis[i].x = x + t * a;
			data2.verticesClosestToTheAxis[i].y = y + t * b;
			data2.verticesClosestToTheAxis[i].z = z + t * c;
		}
		break;
	}
	}

	// Calculations stage

	Vector3 axes[8];

	Vector3* firstObjectNormals = GenerateNormalsForProjection(surface, data1.verticesClosestToTheAxis);
	Vector3* secondObjectNormals = GenerateNormalsForProjection(surface, data2.verticesClosestToTheAxis);

	axes[0] = firstObjectNormals[0];
	axes[1] = firstObjectNormals[1];
	axes[2] = firstObjectNormals[2];
	axes[3] = firstObjectNormals[3];

	axes[4] = secondObjectNormals[0];
	axes[5] = secondObjectNormals[1];
	axes[6] = secondObjectNormals[2];
	axes[7] = secondObjectNormals[3];

	for (int i = 0; i < 8; i++)
	{
		IntervalPair iA = GetInterval2D(axes[i], data1.verticesClosestToTheAxis);
		IntervalPair iB = GetInterval2D(axes[i], data2.verticesClosestToTheAxis);

		if ((iA.imax < iB.imax && iA.imin > iB.imin) ||
			(iB.imax < iA.imax && iB.imin > iA.imin))
		{
			return true;
		}
	}

	return false;
}*/

// surface:
// 0 - OX OZ
// 1 - OX OY
// 2 - OZ OY
Vector3* OBB::GenerateNormalsForProjection(int surface, Vector3* vertices)
{
	Vector3 edges[4];
	edges[0] = vertices[0] - vertices[1];
	edges[1] = vertices[2] - vertices[3];
	edges[2] = vertices[0] - vertices[2];
	edges[3] = vertices[1] - vertices[3];

	Vector3 result[4];

	result[0] = GetPerpendicular2DVector(surface, edges[0]) / GetPerpendicular2DVector(surface, edges[0]).Length();
	result[1] = GetPerpendicular2DVector(surface, edges[1]) / GetPerpendicular2DVector(surface, edges[1]).Length();
	result[2] = GetPerpendicular2DVector(surface, edges[2]) / GetPerpendicular2DVector(surface, edges[2]).Length();
	result[3] = GetPerpendicular2DVector(surface, edges[3]) / GetPerpendicular2DVector(surface, edges[3]).Length();

	return result;
}

// surface:
// 0 - OX OZ
// 1 - OX OY
// 2 - OZ OY
Vector3 OBB::GetPerpendicular2DVector(int surface, Vector3 v)
{
	Vector3 result(0, 0, 0);

	switch (surface)
	{
	case 0:
		result.z = 1.0f;
		result.x = ((-1) * v.z * result.z) / v.x;
		break;
	case 1:
		result.y = 1.0f;
		result.x = ((-1) * v.y * result.y) / v.x;
		break;
	case 2:
		result.y = 1.0f;
		result.z = ((-1) * v.y * result.y) / v.z;
		break;
	}

	return result;
}

IntervalPair OBB::GetInterval2D(Vector3 axis, Vector3* vertices)
{
	IntervalPair result;

	float xx = axis.x;
	float yy = axis.y;
	float zz = axis.z;
	Vector3 v(xx, yy, zz);

	float dot = v.Dot(vertices[0]);
	float imin = dot;
	float imax = dot;

	for (int i = 1; i < 4; i++)
	{
		dot = v.Dot(vertices[i]);
		imin = min(imin, dot);
		imax = max(imax, dot);
	}

	result.imin = imin;
	result.imax = imax;

	return result;
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
