#pragma once
#include <vector>
#include "Rigidbody.h"

class PhysicsEngine
{
public:
	PhysicsEngine() {}

	void AddRigidbody(std::shared_ptr<Rigidbody> rigidbody);
	void Simulate(float delta);
	
private:
	std::vector<std::shared_ptr<Rigidbody>> rigidbodies;
	void HandleCollisions();
};