#pragma once
#include "Collider.h"
#include "BoundingSphere.h"

struct IntervalPair
{
	IntervalPair() {}
	float imin;
	float imax;
	// Only for face to face collision
	//Vector3 verticesClosestToTheAxis[4];
};

class BoundingSphere;

class OBB : public Collider
{
public:
	OBB(Node* owner, Rigidbody* rigidbody, Vector3 center, Vector3 size) :
		Collider(owner, Collider::ColliderTypes::TYPE_OBB, rigidbody),
		center(center),
		size(size)
	{
		//front
		vertices[0] = Vector3(center.x - (size.x / 2.0f), center.y - (size.y / 2.0f), center.z - (size.z / 2.0f));
		vertices[1] = Vector3(center.x + (size.x / 2.0f), center.y - (size.y / 2.0f), center.z - (size.z / 2.0f));
		vertices[2] = Vector3(center.x - (size.x / 2.0f), center.y + (size.y / 2.0f), center.z - (size.z / 2.0f));
		vertices[3] = Vector3(center.x + (size.x / 2.0f), center.y + (size.y / 2.0f), center.z - (size.z / 2.0f));
		//back
		vertices[4] = Vector3(center.x - (size.x / 2.0f), center.y - (size.y / 2.0f), center.z + (size.z / 2.0f));
		vertices[5] = Vector3(center.x + (size.x / 2.0f), center.y - (size.y / 2.0f), center.z + (size.z / 2.0f));
		vertices[6] = Vector3(center.x - (size.x / 2.0f), center.y + (size.y / 2.0f), center.z + (size.z / 2.0f));
		vertices[7] = Vector3(center.x + (size.x / 2.0f), center.y + (size.y / 2.0f), center.z + (size.z / 2.0f));
	}

	IntersectData IntersectOBB(OBB* other);
	IntersectData IntersectBoundingSphere(BoundingSphere* other);
	
	Vector3 NearestPoint(Vector3 otherPoint);
	Vector3 GetTransformedCenter();
	Vector3 GetTransformedSize();
private:
	Vector3 center;
	Vector3 size;
	Vector3 orientationX = Vector3(1.0, 0.0f, 0.0f);
	Vector3 orientationY = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 orientationZ = Vector3(0.0f, 0.0f, 1.0f);
	Vector3 vertices[8];

	Vector3 GetTransformedVertex(Vector3 vertex);
	Vector3 GetClosestPoint(Vector3 point);
	Vector3 GetTransformedOrientation(Vector3 orientation);
	IntervalPair GetInterval(Vector3 axis);
	bool ColOverlapAxis(OBB* first, OBB* second, Vector3 axis);
	bool ProjectAndCheckOverlapping(int surface, IntervalPair data1, IntervalPair data2);
	Vector3* GenerateNormalsForProjection(int surface, Vector3* vertices);
	Vector3 GetPerpendicular2DVector(int surface, Vector3 v);
	IntervalPair GetInterval2D(Vector3 axis, Vector3* vertices);
};