#pragma once
#include <vector>
#include "Rigidbody.h"

class Rigidbody;
class Collider;

class PhysicsEngine
{
public:
	PhysicsEngine() {}

	void AddRigidbody(Rigidbody* rigidbody);
	void AddCollider(Collider* collider);
	void RemoveRigidbody(Rigidbody* rigidbody);
	void RemoveCollider(Collider* collider);
	void Simulate(float delta);
	std::vector<Rigidbody*> rigidbodies;
	std::vector<Collider*> colliders;
	
private:
	void HandleCollisions();
	void HandleRigidbodyCollisions();
	void HandleColliderCollisions();
};