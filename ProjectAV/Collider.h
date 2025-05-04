#pragma once
#include "IntersectData.h"
#include "Rigidbody.h"
#include "ConditionalNoexcept.h"
#include "Raycast.h"
#include "Node.h"
//#include "RaycastData.h"

class Rigidbody;
class Raycast;
class RaycastData;

class Collider : public Component
{
public:
	enum ColliderTypes
	{
		SPHERE = 0,
		AABB = 1,
		TYPE_OBB = 2,
		CAPSULE = 3
	};

	Collider(Node* owner, ColliderTypes colliderType, Rigidbody* rigidbody) :
		colliderType(colliderType),
		rigidbody(rigidbody),
	    Component(owner) {}
	
	void Update(float dt) override;

	IntersectData Intersect(Collider* other);
	virtual RaycastData IntersectRay(Raycast* ray);

	ColliderTypes GetColliderType();

	void SetIsTrigger(bool isTrigger);
	bool GetIsTrigger();
	void SetTriggerEnabled(bool triggerEnabled);
	bool GetTriggerEnabled();

	void AddToTriggerList(Collider* other);
	void UpdateTrigger();

	std::vector<Collider*> GetTriggerEnter();
	std::vector<Collider*> GetTriggerStay();
	std::vector<Collider*> GetTriggerExit();

private:
	ColliderTypes colliderType;
	std::vector<Collider*> collidersThatEnteredTheTrigger;
	std::vector<Collider*> collidersThatLeftTheTrigger;
	std::vector<Collider*> collidersInsideTheTriggerFromPreviousIteration;
	std::vector<Collider*> collidersInsideTheTrigger;
	std::vector<Collider*> stayingColliders;

	void MakeAListOfCollidersThatEnteredTheTrigger();
	void MakeAListOfCollidersThatLeftTheTrigger();
protected:
	Rigidbody* rigidbody;
	bool isTrigger = false;
	bool triggerEnabled = true;
};