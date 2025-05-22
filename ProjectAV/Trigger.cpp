//#include "Trigger.h"
//#include "PhysicsCommon.h"
//
//std::vector<Node*> Trigger::GetTriggerEnter()
//{
//	std::vector<Node*> result;
//
//	for (int i = 0; i < objectsThatEnteredThisFrame.size(); i++)
//	{
//		result.push_back(static_cast<Node*>(objectsThatEnteredThisFrame[i]->getUserPointer()));
//	}
//
//	return result;
//}
//
//std::vector<Node*> Trigger::GetTriggerExit()
//{
//	std::vector<Node*> result;
//
//	for (int i = 0; i < objectsThatExitedThisFrame.size(); i++)
//	{
//		result.push_back(static_cast<Node*>(objectsThatExitedThisFrame[i]->getUserPointer()));
//	}
//
//	return result;
//}
//
//std::vector<Node*> Trigger::GetTriggerStay()
//{
//	std::vector<Node*> result;
//
//	for (int i = 0; i < objectsPresentThisFrame.size(); i++)
//	{
//		result.push_back(static_cast<Node*>(objectsPresentThisFrame[i]->getUserPointer()));
//	}
//
//	return result;
//}
//
//void Trigger::Update(float dt)
//{
//	btTransform transform;
//	transform.setIdentity();
//	transform.setOrigin(btVector3(pOwner->GetWorldPosition().x,
//									pOwner->GetWorldPosition().y,
//									pOwner->GetWorldPosition().z));
//	bulletGhostObject->setWorldTransform(transform);
//
//	objectsThatEnteredThisFrame.clear();
//	objectsThatExitedThisFrame.clear();
//	objectsPresentThisFrame.clear();
//
//	int collisions = bulletGhostObject->getNumOverlappingObjects();
//	for (int i = 0; i < collisions; i++)
//	{
//		btCollisionObject* collisionObject = bulletGhostObject->getOverlappingObject(i);
//
//		objectsPresentThisFrame.push_back(collisionObject);
//
//		if (std::count(objectsPresentLastFrame.begin(), objectsPresentLastFrame.end(), collisionObject) == 0)
//		{
//			objectsThatEnteredThisFrame.push_back(collisionObject);
//		}
//	}
//
//	for (int i = 0; i < objectsPresentLastFrame.size(); i++)
//	{
//		btCollisionObject* collisionObject = objectsPresentLastFrame[i];
//
//		if (std::count(objectsPresentThisFrame.begin(), objectsPresentThisFrame.end(), collisionObject) == 0)
//		{
//			objectsThatExitedThisFrame.push_back(collisionObject);
//		}
//	}
//
//	objectsPresentLastFrame = objectsPresentThisFrame;
//}
