#pragma once
#include <SimpleMath.h>

#include "PhysicsEngine.h"
#include "RaycastData.h"

using namespace DirectX::SimpleMath;

class PhysicsEngine;
class RaycastData;

class Raycast
{
public:
	Raycast(Vector3 origin, Vector3 direction) : origin(origin), direction(direction) {}

	static RaycastData Cast(Vector3 origin, Vector3 direction);
	static PhysicsEngine* physicsEngine;

	Vector3 origin;
	Vector3 direction;
};