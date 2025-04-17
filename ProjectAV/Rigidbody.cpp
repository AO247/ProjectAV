#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

Vector3 Rigidbody::gravity = Vector3(0.0f, 0.0f, 0.0f);

void Rigidbody::Update(float dt)
{
	//pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
	pOwner->SetWorldPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
}

void Rigidbody::Integrate(float delta)
{
	//position += (velocity * delta);

	//DirectX::XMVECTOR positionVec = GetOwner()->GetWorldTransform().r[3];
	//DirectX::XMFLOAT3 pos;
	//XMStoreFloat3(&pos, positionVec);
	//Vector3 v(pos.x, pos.y, pos.z);

	Vector3 pos(GetOwner()->GetWorldPosition().x,
				GetOwner()->GetWorldPosition().y,
				GetOwner()->GetWorldPosition().z);
	//position = pos;

	force += mass * gravity;
	velocity -= velocity * linearVelocityDamping;
	velocity += (force / mass) * delta;
	position = pos + (velocity * delta);

	force = Vector3(0, 0, 0);
}

void Rigidbody::SetPosition(Vector3 position)
{
	this->position = position;
}

void Rigidbody::SetCollider(Collider* collider)
{
	this->collider = collider;
}

void Rigidbody::SetVelocity(Vector3 velocity)
{
	this->velocity = velocity;
}

void Rigidbody::AddForce(Vector3 force)
{
	this->force += force;
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

float Rigidbody::GetMass()
{
	return mass;
}
