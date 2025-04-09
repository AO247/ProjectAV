#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

void Rigidbody::Update(float dt)
{
	pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
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



	position = (pos + position) + (velocity * delta);
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