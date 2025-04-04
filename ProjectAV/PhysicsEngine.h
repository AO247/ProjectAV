#pragma once
#include <vector>
#include "Rigidbody.h"

class PhysicsEngine
{
public:
	PhysicsEngine() {}

	void AddRigidbody(Rigidbody& rigidbody);
	void Simulate(float delta);
	void HandleCollisions();
private:
	std::vector<Rigidbody> rigidbodies;
};

