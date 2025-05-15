//#pragma once
//#include <SimpleMath.h>
//
//#include "PhysicsEngine.h"
//#include "RaycastData.h"
//#include "PhysicsLayers.h"
//
//using namespace DirectX::SimpleMath;
//
//class PhysicsEngine;
//class RaycastData;
//
//class Raycast
//{
//public:
//	Raycast(Vector3 origin, Vector3 direction) : origin(origin), direction(direction) {}
//
//	static RaycastData Cast(Vector3 origin, Vector3 direction);
//	static RaycastData CastAtLayers(Vector3 origin, Vector3 direction, std::vector<Layers> layersToIntersect);
//	static RaycastData CastThroughLayers(Vector3 origin, Vector3 direction, std::vector<Layers> layersToAvoid);
//	static PhysicsEngine* physicsEngine;
//
//	Vector3 origin;
//	Vector3 direction;
//
//private:
//	static bool CheckIfColliderIsInLayers(Collider* collider, std::vector<Layers> layers);
//};