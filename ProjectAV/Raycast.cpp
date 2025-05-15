//#include "Raycast.h"
//
//PhysicsEngine* Raycast::physicsEngine = nullptr;
//
//RaycastData Raycast::Cast(Vector3 origin, Vector3 direction)
//{
//	RaycastData castResult(nullptr, Vector3(0, 0, 0));
//
//	Raycast ray(origin, direction);
//	for (int i = 0; i < physicsEngine->rigidbodies.size(); i++)
//	{
//		RaycastData rayData = physicsEngine->rigidbodies[i]->GetCollider()->IntersectRay(&ray);
//		if (rayData.hitCollider != nullptr)
//		{
//			if (castResult.hitCollider == nullptr)
//			{
//				castResult = rayData;
//				continue;
//			}
//			else
//			{
//				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
//				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
//
//				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
//				{
//					castResult = rayData;
//				}
//			}
//		}
//	}
//	for (int i = 0; i < physicsEngine->colliders.size(); i++)
//	{
//		RaycastData rayData = physicsEngine->colliders[i]->IntersectRay(&ray);
//		if (rayData.hitCollider != nullptr)
//		{
//			if (castResult.hitCollider == nullptr)
//			{
//				castResult = rayData;
//				continue;
//			}
//			else
//			{
//				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
//				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
//				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
//				{
//					castResult = rayData;
//				}
//			}
//		}
//	}
//
//	return castResult;
//}
//
//RaycastData Raycast::CastAtLayers(Vector3 origin, Vector3 direction, std::vector<Layers> layersToIntersect)
//{
//	RaycastData castResult(nullptr, Vector3(0, 0, 0));
//
//	Raycast ray(origin, direction);
//	for (int i = 0; i < physicsEngine->rigidbodies.size(); i++)
//	{
//		RaycastData rayData = physicsEngine->rigidbodies[i]->GetCollider()->IntersectRay(&ray);
//		if (rayData.hitCollider != nullptr)
//		{
//			if (!CheckIfColliderIsInLayers(rayData.hitCollider, layersToIntersect))
//			{
//				continue;
//			}
//			if (castResult.hitCollider == nullptr)
//			{
//				castResult = rayData;
//				continue;
//			}
//			else
//			{
//				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
//				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
//
//				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
//				{
//					castResult = rayData;
//				}
//			}
//		}
//	}
//	for (int i = 0; i < physicsEngine->colliders.size(); i++)
//	{
//		RaycastData rayData = physicsEngine->colliders[i]->IntersectRay(&ray);
//		if (rayData.hitCollider != nullptr)
//		{
//			if (!CheckIfColliderIsInLayers(rayData.hitCollider, layersToIntersect))
//			{
//				continue;
//			}
//			if (castResult.hitCollider == nullptr)
//			{
//				castResult = rayData;
//				continue;
//			}
//			else
//			{
//				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
//				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
//				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
//				{
//					castResult = rayData;
//				}
//			}
//		}
//	}
//
//	return castResult;
//}
//
//
//RaycastData Raycast::CastThroughLayers(Vector3 origin, Vector3 direction, std::vector<Layers> layersToAvoid)
//{
//	RaycastData castResult(nullptr, Vector3(0, 0, 0));
//
//	Raycast ray(origin, direction);
//	for (int i = 0; i < physicsEngine->rigidbodies.size(); i++)
//	{
//		RaycastData rayData = physicsEngine->rigidbodies[i]->GetCollider()->IntersectRay(&ray);
//		if (rayData.hitCollider != nullptr)
//		{
//			if (CheckIfColliderIsInLayers(rayData.hitCollider, layersToAvoid))
//			{
//				continue;
//			}
//			if (castResult.hitCollider == nullptr)
//			{
//				castResult = rayData;
//				continue;
//			}
//			else
//			{
//				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
//				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
//
//				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
//				{
//					castResult = rayData;
//				}
//			}
//		}
//	}
//	for (int i = 0; i < physicsEngine->colliders.size(); i++)
//	{
//		RaycastData rayData = physicsEngine->colliders[i]->IntersectRay(&ray);
//		if (rayData.hitCollider != nullptr)
//		{
//			if (CheckIfColliderIsInLayers(rayData.hitCollider, layersToAvoid))
//			{
//				continue;
//			}
//			if (castResult.hitCollider == nullptr)
//			{
//				castResult = rayData;
//				continue;
//			}
//			else
//			{
//				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
//				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
//				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
//				{
//					castResult = rayData;
//				}
//			}
//		}
//	}
//
//	return castResult;
//}
//
//
//bool Raycast::CheckIfColliderIsInLayers(Collider* collider, std::vector<Layers> layers)
//{
//	for (int i = 0; i < layers.size(); i++)
//	{
//		if (collider->GetLayer() == layers[i])
//		{
//			return true;
//		}
//	}
//
//	return false;
//}
