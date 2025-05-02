#include "Rigidbody.h"
#include "Node.h"
#include "DirectXMath.h"

Vector3 Rigidbody::gravity = Vector3(0.0f, -98.0f, 0.0f);

void Rigidbody::Update(float dt)
{
	//pOwner->SetLocalPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
	pOwner->SetWorldPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
	pOwner->SetLocalRotation(DirectX::XMFLOAT3(rotation.x, rotation.y, rotation.z));
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

	if (!isStatic)
	{
		force += mass * gravity;
	}
	velocity -= velocity * linearVelocityDamping * delta;
	velocity += (force / mass) * delta;
	position = pos + (velocity * delta);

	Vector3 rot(GetOwner()->GetLocalRotationEuler().x,
				GetOwner()->GetLocalRotationEuler().y,
				GetOwner()->GetLocalRotationEuler().z);

	angularVelocity -= angularVelocity * angularVelocityDamping * delta;
	rotation = rot + (angularVelocity * delta);

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

void Rigidbody::SetMass(float mass)
{
	this->mass = mass;
}

void Rigidbody::AddForce(Vector3 force)
{
	this->force += force;
}

void Rigidbody::SetStatic(bool isStatic)
{
	this->isStatic = isStatic;
}

void Rigidbody::SetAngularVelocity(Vector3 velocity)
{
	angularVelocity = velocity;
}

Vector3& Rigidbody::GetPosition()
{
	return position;
}

Vector3& Rigidbody::GetVelocity()
{
	return velocity;
}

Vector3& Rigidbody::GetRotation()
{
	return rotation;
}

Vector3& Rigidbody::GetAngularVelocity()
{
	return angularVelocity;
}

Collider* Rigidbody::GetCollider()
{
	return collider;
}

DirectX::XMMATRIX Rigidbody::GetTransformationMatrixFromNode()
{
	return pOwner->GetWorldTransform();
}

DirectX::XMMATRIX Rigidbody::GetBodyTransformationMatrix()
{
	DirectX::XMMATRIX result = DirectX::XMMatrixIdentity();
	
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z);
	DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(pOwner->GetLocalScale().x, pOwner->GetLocalScale().y, pOwner->GetLocalScale().z);

	result = scale * rotation * translation;

	return result;
}

DirectX::XMFLOAT3 Rigidbody::GetScaleFromNode()
{
	return pOwner->GetLocalScale();
}

DirectX::XMFLOAT3 Rigidbody::GetRotationFromNode()
{
	return pOwner->GetLocalRotationEuler();
}

float Rigidbody::GetMass()
{
	return mass;
}

bool Rigidbody::GetIsStatic()
{
	return isStatic;
}
