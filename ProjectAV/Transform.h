#pragma once
#include "Component.h"
#include <SimpleMath.h>
#include <DirectXMath.h>

using namespace DirectX::SimpleMath;

class Transform : public Component
{
public:
	void Update() override;
	void SetPosition(Vector3& position);
	void SetRotation(Vector3& rotation);
	void SetScale(Vector3& scale);
	Vector3& GetPosition();
	Vector3& GetRotation();
	Vector3& GetScale();
	DirectX::XMMATRIX GetTransformationMatrix();
private:
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 rotation = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 scale = Vector3(1.0f, 1.0f, 1.0f);
};

