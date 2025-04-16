#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class Collider;

class Rigidbody : public Component
{
public:
	Rigidbody() : Component(nullptr) {}
	Rigidbody(Node* owner, Vector3 position, Vector3 velocity) :
		position(position),
		velocity(velocity),
		Component(owner) {}

	void Update(float dt) override;
	void Integrate(float delta);

	void SetPosition(Vector3 position);
	void SetCollider(Collider* collider);
	void SetVelocity(Vector3 velocity);
	void AddForce(Vector3 force);

	Vector3& GetPosition();
	Vector3& GetVelocity();
	Collider* GetCollider();
	float GetMass();

	static Vector3 gravity;

private:
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float mass = 10.0f;
	Collider* collider;
};