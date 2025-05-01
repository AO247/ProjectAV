#pragma once
#include "IntersectData.h"
#include "Rigidbody.h"
#include "ConditionalNoexcept.h"

class Rigidbody;

class Collider : public Component
{
public:
	enum ColliderTypes
	{
		SPHERE = 0,
		AABB = 1,
		TYPE_OBB = 2
	};

	Collider(Node* owner, ColliderTypes colliderType, Rigidbody* rigidbody) :
		colliderType(colliderType),
		rigidbody(rigidbody),
	    Component(owner) {}
	
	void Update(float dt) override;

	IntersectData Intersect(Collider* other);

	ColliderTypes GetColliderType();

	Rigidbody* GetRigidbody() const;

private:
	ColliderTypes colliderType;
protected:
	Rigidbody* rigidbody;
};