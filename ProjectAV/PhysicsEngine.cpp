#include "PhysicsEngine.h"
#include <sstream>

void PhysicsEngine::AddRigidbody(Rigidbody* rigidbody)
{
	rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::Simulate(float delta)
{
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		rigidbodies[i]->Integrate(delta);
	}
	HandleCollisions();
}

void PhysicsEngine::HandleCollisions()
{
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		for (int j = i + 1; j < rigidbodies.size(); j++)
		{
			IntersectData intersectData = rigidbodies[i]->GetCollider()->Intersect(
				rigidbodies[j]->GetCollider());
			if (intersectData.GetDoesIntersect())
			{
				//
				//
				// SEPARATION
				//
				//

				// Collision for moving and not moving body OR both not moving
				if (rigidbodies[i]->GetVelocity().Length() * rigidbodies[j]->GetVelocity().Length() == 0)
				{
					if (rigidbodies[i]->GetVelocity().Length() != 0)
					{
						Vector3 firstBodyPositionAfterSeparation = rigidbodies[i]->GetPosition() + intersectData.GetDirection();
						rigidbodies[i]->SetPosition(firstBodyPositionAfterSeparation);
					}
					else
					{
						Vector3 secondBodyPositionAfterSeparation = rigidbodies[j]->GetPosition() - intersectData.GetDirection();
						rigidbodies[j]->SetPosition(secondBodyPositionAfterSeparation);
					}
				}
				// Collision for moving and moving body
				else
				{
					Vector3 halfSeparationVector = intersectData.GetDirection() / 2.0f;
					Vector3 firstBodyPositionAfterSeparation = rigidbodies[i]->GetPosition() + halfSeparationVector;
					Vector3 secondBodyPositionAfterSeparation = rigidbodies[j]->GetPosition() - halfSeparationVector;
					rigidbodies[i]->SetPosition(firstBodyPositionAfterSeparation);
					rigidbodies[j]->SetPosition(secondBodyPositionAfterSeparation);
				}

				//
				//
				// Combining forces
				//
				//
				float e = 1.0f;
				Vector3 relativeVelocity = rigidbodies[i]->GetVelocity() - rigidbodies[j]->GetVelocity();
				float factor = ( (-(1 + e) * relativeVelocity)).Dot(intersectData.GetDirection() ) / 
						  ( intersectData.GetDirection().Dot( intersectData.GetDirection() * (1/rigidbodies[i]->GetMass() + 1/rigidbodies[j]->GetMass()) ) );

				Vector3 firstBodyNewVelocity = rigidbodies[i]->GetVelocity() + ((factor / rigidbodies[i]->GetMass()) * intersectData.GetDirection());
				Vector3 secondBodyNewVelocity = rigidbodies[j]->GetVelocity() - ((factor / rigidbodies[j]->GetMass()) * intersectData.GetDirection());

				rigidbodies[i]->SetVelocity(firstBodyNewVelocity);
				rigidbodies[j]->SetVelocity(secondBodyNewVelocity);
			}
		}
	}
}