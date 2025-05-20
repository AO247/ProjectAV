#pragma once
#include "btBulletDynamicsCommon.h"
#include "vector"
#include "SimpleMath.h"
#include "ModelComponent.h"

using namespace DirectX::SimpleMath;

class PhysicsCommon
{
public:
	static btDynamicsWorld* dynamicsWorld;
	static btConvexHullShape* MakeConvexHullShape(std::vector<Vector3> vertices)
	{
		btConvexHullShape* result = new btConvexHullShape();
		for (int i = 0; i < vertices.size(); i++)
		{
			result->addPoint(btVector3(vertices[i].x, vertices[i].y, vertices[i].z));
		}
		return result;
	}
	// ONLY FOR STATIC OBJECTS
	static btBvhTriangleMeshShape* MakeConcaveShape(std::vector<ModelComponent::Triangle> triangles)
	{
		btTriangleMesh* triangleMesh = new btTriangleMesh();
		for (int i = 0; i < triangles.size(); i++)
		{
			triangleMesh->addTriangle(btVector3(triangles[i].v0.x, triangles[i].v0.y, triangles[i].v0.z),
										btVector3(triangles[i].v1.x, triangles[i].v1.y, triangles[i].v1.z),
										btVector3(triangles[i].v2.x, triangles[i].v2.y, triangles[i].v2.z));
		}
		btBvhTriangleMeshShape* meshShape = new btBvhTriangleMeshShape(triangleMesh, true);

		return meshShape;
	}
};