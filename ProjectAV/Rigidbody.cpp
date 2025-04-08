#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

void Rigidbody::Update(float dt)
{
	pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
}

void Rigidbody::Integrate(float delta)
{
	position += (velocity * delta);
}

void Rigidbody::SetPosition(Vector3 position)
{
	this->position = position;
}

void Rigidbody::SetCollider(Collider* collider)
{
	this->collider = collider;
}

Vector3& Rigidbody::GetPosition()
{
	return position;
}

Vector3& Rigidbody::GetVelocity()
{
	return velocity;
}

Collider* Rigidbody::GetCollider()
{
	return collider;
}