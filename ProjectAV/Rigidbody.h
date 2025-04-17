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
	void SetMass(float mass);
	void AddForce(Vector3 force);
	void SetStatic(bool isStatic);

	Vector3& GetPosition();
	Vector3& GetVelocity();
	Collider* GetCollider();
	float GetMass();
	bool GetIsStatic();

	static Vector3 gravity;

private:
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float mass = 10.0f;
	float linearVelocityDamping = 0.01f;
	bool isStatic = false;
	Collider* collider;
};