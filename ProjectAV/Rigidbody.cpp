#include "Rigidbody.h"

void Rigidbody::Update()
{
	transform->SetPosition(position);
}

void Rigidbody::Integrate(float delta)
{
	position += velocity * delta;
}

void Rigidbody::SetPosition(Vector3& position)
{
	this->position = position;
}

Vector3& Rigidbody::GetPosition()
{
	return position;
}

Vector3& Rigidbody::GetVelocity()
{
	return velocity;
}

Collider& Rigidbody::GetCollider()
{
	return collider;
}
