#include "Raycast.h"

PhysicsEngine* Raycast::physicsEngine = nullptr;

RaycastData Raycast::Cast(Vector3 origin, Vector3 direction)
{
	RaycastData castResult(nullptr, Vector3(0, 0, 0));

	Raycast ray(origin, direction);
	for (int i = 0; i < physicsEngine->rigidbodies.size(); i++)
	{
		RaycastData rayData = physicsEngine->rigidbodies[i]->GetCollider()->IntersectRay(&ray);
		if (rayData.hitCollider != nullptr)
		{
			if (castResult.hitCollider == nullptr)
			{
				castResult = rayData;
				continue;
			}
			else
			{
				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();

				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
				{
					castResult = rayData;
				}
			}
		}
	}
	for (int i = 0; i < physicsEngine->colliders.size(); i++)
	{
		RaycastData rayData = physicsEngine->colliders[i]->IntersectRay(&ray);
		if (rayData.hitCollider != nullptr)
		{
			if (castResult.hitCollider == nullptr)
			{
				castResult = rayData;
				continue;
			}
			else
			{
				float distanceFromOriginToCastResult = (origin - castResult.hitPoint).Length();
				float distanceFromOriginToRayData = (origin - rayData.hitPoint).Length();
				if (distanceFromOriginToRayData < distanceFromOriginToCastResult)
				{
					castResult = rayData;
				}
			}
		}
	}

	return castResult;
}
