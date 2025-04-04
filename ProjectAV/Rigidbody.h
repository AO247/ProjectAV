#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Transform.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class Rigidbody : public Component
{
public:
	Rigidbody(Transform* transform, Collider& collider, Vector3& position, Vector3& velocity) :
		position(position),
		velocity(velocity),
		transform(transform),
		collider(collider) {}

	void Update() override;
	void Integrate(float delta);

	void SetPosition(Vector3& position);

	Vector3& GetPosition();
	Vector3& GetVelocity();
	Collider& GetCollider();
private:
	Vector3 position;
	Vector3 velocity;
	Transform* transform;
	Collider collider;
};

