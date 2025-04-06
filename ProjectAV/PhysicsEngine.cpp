#include "PhysicsEngine.h"

void PhysicsEngine::AddRigidbody(Rigidbody& rigidbody)
{
	rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::Simulate(float delta)
{
	for (Rigidbody rigidbody : rigidbodies)
	{
		rigidbody.Integrate(delta);
	}
	HandleCollisions();
}

void PhysicsEngine::HandleCollisions()
{
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		for (int j = i + 1; j < rigidbodies.size(); j++)
		{
			IntersectData intersectData = rigidbodies[i].GetCollider().Intersect(
				rigidbodies[j].GetCollider());
			if (intersectData.GetDoesIntersect())
			{
				Vector3 firstBodyPositionAfterSeparation = rigidbodies[i].GetPosition() + (intersectData.GetDirection() * -1);
				rigidbodies[i].SetPosition(firstBodyPositionAfterSeparation);
			}
		}
	}
}