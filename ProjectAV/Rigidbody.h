#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class Rigidbody : public Component
{
public:
	Rigidbody(Node* owner, Collider& collider, Vector3& position, Vector3& velocity) :
		position(position),
		velocity(velocity),
		Component(owner),
		collider(collider) {
	}

	void Update(float dt) override;
	void Integrate(float delta);

	void SetPosition(Vector3& position);

	Vector3& GetPosition();
	Vector3& GetVelocity();
	Collider& GetCollider();
private:
	Vector3 position;
	Vector3 velocity;
	Collider collider;
};