#include "Collider.h"
#include "BoundingSphere.h"
#include "AABB.h"

void Collider::Update(float dt)
{
}

IntersectData Collider::Intersect(Collider* other)
{
	if (colliderType == SPHERE && other->GetColliderType() == SPHERE)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectBoundingSphere((BoundingSphere*)other);
	}
	if (colliderType == SPHERE && other->GetColliderType() == TYPE_OBB)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectOBB((OBB*)other);
	}
	if (colliderType == SPHERE && other->GetColliderType() == CAPSULE)
	{
		BoundingSphere* self = (BoundingSphere*)this;
		return self->IntersectCapsule((CapsuleCollider*)other);
	}
	if (colliderType == TYPE_OBB && other->GetColliderType() == TYPE_OBB)
	{
		OBB* self = (OBB*)this;
		return self->IntersectOBB((OBB*)other);
	}
	if (colliderType == TYPE_OBB && other->GetColliderType() == SPHERE)
	{
		OBB* self = (OBB*)this;
		return self->IntersectBoundingSphere((BoundingSphere*)other);
	}
	if (colliderType == TYPE_OBB && other->GetColliderType() == CAPSULE)
	{
		OBB* self = (OBB*)this;
		return self->IntersectCapsule((CapsuleCollider*)other);
	}
	if (colliderType == CAPSULE && other->GetColliderType() == CAPSULE)
	{
		CapsuleCollider* self = (CapsuleCollider*)this;
		return self->IntersectCapsule((CapsuleCollider*)other);
	}
	if (colliderType == CAPSULE && other->GetColliderType() == SPHERE)
	{
		CapsuleCollider* self = (CapsuleCollider*)this;
		return self->IntersectSphere((BoundingSphere*)other);
	}
	if (colliderType == CAPSULE && other->GetColliderType() == TYPE_OBB)
	{
		CapsuleCollider* self = (CapsuleCollider*)this;
		return self->IntersectOBB((OBB*)other);
	}
}

RaycastData Collider::IntersectRay(Raycast* ray)
{
	return RaycastData(nullptr, Vector3(0, 0, 0));
}

Collider::ColliderTypes Collider::GetColliderType()
{
	return colliderType;
}

void Collider::SetIsTrigger(bool isTrigger)
{
	this->isTrigger = isTrigger;
}

bool Collider::GetIsTrigger()
{
	return isTrigger;
}

void Collider::SetTriggerEnabled(bool triggerEnabled)
{
	this->triggerEnabled = triggerEnabled;
	collidersThatEnteredTheTrigger.clear();
	collidersThatLeftTheTrigger.clear();
	collidersInsideTheTriggerFromPreviousIteration.clear();
	collidersInsideTheTrigger.clear();
	stayingColliders.clear();
}

bool Collider::GetTriggerEnabled()
{
	return triggerEnabled;
}

void Collider::UpdateTrigger()
{
	if (triggerEnabled)
	{
		MakeAListOfCollidersThatEnteredTheTrigger();
		MakeAListOfCollidersThatLeftTheTrigger();
		stayingColliders = collidersInsideTheTrigger;
		collidersInsideTheTriggerFromPreviousIteration = collidersInsideTheTrigger;
		collidersInsideTheTrigger.clear();
	}
}

std::vector<Collider*> Collider::GetTriggerEnter()
{
	return collidersThatEnteredTheTrigger;
}

std::vector<Collider*> Collider::GetTriggerStay()
{
	return stayingColliders;
}

std::vector<Collider*> Collider::GetTriggerExit()
{
	return collidersThatLeftTheTrigger;
}

void Collider::AddToTriggerList(Collider* other)
{
	if (triggerEnabled)
	{
		collidersInsideTheTrigger.push_back(other);
	}
}

void Collider::MakeAListOfCollidersThatEnteredTheTrigger()
{
	collidersThatEnteredTheTrigger.clear();

	for (int i = 0; i < collidersInsideTheTrigger.size(); i++)
	{
		bool theColliderIsNewForTheTrigger = true;
		for (int j = 0; j < collidersInsideTheTriggerFromPreviousIteration.size(); j++)
		{
			if (collidersInsideTheTriggerFromPreviousIteration[j] == collidersInsideTheTrigger[i])
			{
				theColliderIsNewForTheTrigger = false;
				break;
			}
		}
		if (theColliderIsNewForTheTrigger)
		{
			collidersThatEnteredTheTrigger.push_back(collidersInsideTheTrigger[i]);
		}
	}
}

void Collider::MakeAListOfCollidersThatLeftTheTrigger()
{
	collidersThatLeftTheTrigger.clear();

	for (int i = 0; i < collidersInsideTheTriggerFromPreviousIteration.size(); i++)
	{
		bool theColliderIsMissing = true;
		for (int j = 0; j < collidersInsideTheTrigger.size(); j++)
		{
			if (collidersInsideTheTrigger[j] == collidersInsideTheTriggerFromPreviousIteration[i])
			{
				theColliderIsMissing = false;
				break;
			}
		}
		if (theColliderIsMissing)
		{
			collidersThatLeftTheTrigger.push_back(collidersInsideTheTriggerFromPreviousIteration[i]);
		}
	}
}
