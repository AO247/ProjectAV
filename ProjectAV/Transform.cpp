#include "Transform.h"

void Transform::Update()
{
}

void Transform::SetPosition(Vector3& position)
{
	this->position = position;
}

void Transform::SetRotation(Vector3& rotation)
{
	this->rotation = rotation;
}

void Transform::SetScale(Vector3& scale)
{
	this->scale = scale;
}

Vector3& Transform::GetPosition()
{
	return position;
}

Vector3& Transform::GetRotation()
{
	return rotation;
}

Vector3& Transform::GetScale()
{
	return scale;
}

DirectX::XMMATRIX Transform::GetTransformationMatrix()
{
	DirectX::XMMATRIX transformationMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.z, rotation.y, rotation.x);
	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix * transformationMatrix;
	return transformationMatrix;
}
