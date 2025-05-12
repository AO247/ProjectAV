#pragma once
#include "Collider.h"
#include "BoundingSphere.h"

class IntervalPair
{
public:
	IntervalPair() {}
	float imin = 0;
	float imax = 0;
	// Only for face to face collision
	//Vector3 verticesClosestToTheAxis[4] = {Vector3(0,0,0), Vector3(0,0,0) , Vector3(0,0,0) , Vector3(0,0,0) };
};

class BoundingSphere;
class CapsuleCollider;
class MeshCollider;

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
	IntersectData IntersectCapsule(CapsuleCollider* other);
	IntersectData IntersectMesh(MeshCollider* other);
	RaycastData IntersectRay(Raycast* ray) override;
	
	Vector3 GetTransformedSize();

	Vector3 NearestPoint(Vector3 otherPoint);
	Vector3 GetTransformedCenter();
	Vector3 GetClosestPoint(Vector3 point);
	Vector3 GetColliderTransformedCenter() override;
	Vector3 GetTransformedOrientation(Vector3 orientation);
	Vector3 orientationX = Vector3(1.0, 0.0f, 0.0f);
	Vector3 orientationY = Vector3(0.0f, 1.0f, 0.0f);
	Vector3 orientationZ = Vector3(0.0f, 0.0f, 1.0f);

	enum Axes
	{
		X = 0,
		Y = 1,
		Z = 2,
		NEGATIVE_X = 3,
		NEGATIVE_Y = 4,
		NEGATIVE_Z = 5
	};

	struct MyPlane
	{
		Vector3 normal;
		float offset;
	};

	std::vector<Vector3> GetVerticesOfFacePointedByAxis(Axes axis);
	Axes GetAxisClosestToTheDirection(Vector3 direction);
	std::vector<Vector3> ClipPolygon(std::vector<Vector3> polygon, MyPlane plane);
	std::vector<MyPlane> GetFaceSidePlanes(Vector3 faceCenter, Vector3 u, Vector3 v, float hu, float hv);
	std::vector<Vector3> ClipIncidentFaceToReferenceFace(std::vector<Vector3> incidentFaceVerts, std::vector<MyPlane> refSidePlanes);
	Vector3 AverageContactPoint(std::vector<Vector3> contactPoints);
private:
	Vector3 center;
	Vector3 size;
	Vector3 vertices[8];

	Vector3 GetTransformedVertex(Vector3 vertex);
	IntervalPair GetInterval(Vector3 axis);
	bool ColOverlapAxis(OBB* first, OBB* second, Vector3 axis);
	//bool ProjectAndCheckOverlapping(int surface, IntervalPair data1, IntervalPair data2);
	Vector3* GenerateNormalsForProjection(int surface, Vector3* vertices);
	Vector3 GetPerpendicular2DVector(int surface, Vector3 v);
	IntervalPair GetInterval2D(Vector3 axis, Vector3* vertices);
};