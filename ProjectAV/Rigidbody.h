#pragma once
#include <SimpleMath.h>
#include "Component.h"
#include "Collider.h"

using namespace DirectX::SimpleMath;

class PhysicsEngine;
class Collider;

class Rigidbody : public Component
{
public:
	Rigidbody() : Component(nullptr) {}
	Rigidbody(Node* owner, Vector3 position, Vector3 velocity) :
		position(position),
		velocity(velocity),
		Component(owner) {}
	virtual ~Rigidbody() = default;

	void Update(float dt) override;
	void Integrate(float delta);

	void SetPosition(Vector3 position);
	void SetCollider(Collider* collider);
	void SetVelocity(Vector3 velocity);
	void SetMass(float mass);
	void AddForce(Vector3 force);
	void SetStatic(bool isStatic);
	void SetAngularVelocity(Vector3 velocity);
	void GroundCheck();
	Vector3& GetPosition();
	Vector3& GetVelocity();
	Vector3& GetRotation();
	Vector3& GetAngularVelocity();
	Collider* GetCollider();
	DirectX::XMMATRIX GetTransformationMatrixFromNode();
	DirectX::XMMATRIX GetBodyTransformationMatrix();
	DirectX::XMFLOAT3 GetScaleFromNode();
	DirectX::XMFLOAT3 GetRotationFromNode();
	float GetMass();
	bool GetIsStatic();

	static Vector3 gravity;
	bool grounded = false;
	bool friction = true;
	float frictionDamping = 5.0f;
	virtual void DrawImGuiControls() override;

private:
	Vector3 position;
	Vector3 rotation;
	Vector3 velocity;
	Vector3 angularVelocity;
	Vector3 force;

	float mass = 10.0f;
	float linearVelocityDamping = 3.0f;
	float angularVelocityDamping = 1.0f;
	bool isStatic = false;
	Collider* collider;
};