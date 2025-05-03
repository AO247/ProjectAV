#pragma once
#include <vector>
#include "Rigidbody.h"

class Rigidbody;

class PhysicsEngine
{
public:
	PhysicsEngine() {}

	void AddRigidbody(Rigidbody* rigidbody);
	void Simulate(float delta);
	std::vector<Rigidbody*> rigidbodies;
	
private:
	void HandleCollisions();
};