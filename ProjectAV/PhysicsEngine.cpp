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
			// firstBody must contain a moving body
			// in order to solve collisions properly
			Rigidbody* firstBody, * secondBody;

			firstBody = rigidbodies[i];
			secondBody = rigidbodies[j];

			if (rigidbodies[j]->GetVelocity().Length() != 0)
			{
				firstBody = rigidbodies[j];
				secondBody = rigidbodies[i];
			}

			IntersectData intersectData = firstBody->GetCollider()->Intersect(
										  secondBody->GetCollider());

			if (intersectData.GetDoesIntersect())
			{

				//
				//
				// SEPARATION
				//
				//

				// Collision for moving and not moving body OR both not moving (including static cases)
				if (firstBody->GetVelocity().Length() * secondBody->GetVelocity().Length() == 0)
				{
					if (firstBody->GetVelocity().Length() != 0)
					{
						Vector3 firstBodyPositionAfterSeparation = firstBody->GetPosition() + intersectData.GetDirection();
						firstBody->SetPosition(firstBodyPositionAfterSeparation);
					}
					else
					{
						Vector3 secondBodyPositionAfterSeparation = secondBody->GetPosition() - intersectData.GetDirection();
						secondBody->SetPosition(secondBodyPositionAfterSeparation);
					}
				}
				// Collision for moving and moving body
				else
				{
					Vector3 halfSeparationVector = intersectData.GetDirection() / 2.0f;
					Vector3 firstBodyPositionAfterSeparation = firstBody->GetPosition() + halfSeparationVector;
					Vector3 secondBodyPositionAfterSeparation = secondBody->GetPosition() - halfSeparationVector;
					firstBody->SetPosition(firstBodyPositionAfterSeparation);
					secondBody->SetPosition(secondBodyPositionAfterSeparation);
				}

				//
				//
				// Combining forces
				//
				//
				
				float e = 0.0f;
				Vector3 relativeVelocity = firstBody->GetVelocity() - secondBody->GetVelocity();
				float factor = ( (-(1 + e) * relativeVelocity)).Dot(intersectData.GetDirection() ) / 
						  ( intersectData.GetDirection().Dot( intersectData.GetDirection() * (1/firstBody->GetMass() + 1/secondBody->GetMass()) ) );

				Vector3 firstBodyNewVelocity = firstBody->GetVelocity() + ((factor / firstBody->GetMass()) * intersectData.GetDirection());
				Vector3 secondBodyNewVelocity = secondBody->GetVelocity() - ((factor / secondBody->GetMass()) * intersectData.GetDirection());

				firstBody->SetVelocity(firstBodyNewVelocity);
				secondBody->SetVelocity(secondBodyNewVelocity);

				if (firstBody->GetIsStatic())
				{
					firstBody->SetVelocity(Vector3(0, 0, 0));
				}
				if (secondBody->GetIsStatic())
				{
					secondBody->SetVelocity(Vector3(0, 0, 0));
				}
			}
		}
	}
}