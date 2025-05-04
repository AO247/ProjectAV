#include "PhysicsEngine.h"
#include <sstream>

void PhysicsEngine::AddRigidbody(Rigidbody* rigidbody)
{
	rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::AddCollider(Collider* collider)
{
	colliders.push_back(collider);
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
	HandleRigidbodyCollisions();
	HandleColliderCollisions();
}

void PhysicsEngine::HandleRigidbodyCollisions()
{
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		for (int j = i + 1; j < rigidbodies.size(); j++)
		{
			if (rigidbodies[i]->GetIsStatic() && rigidbodies[j]->GetIsStatic())
			{
				continue;
			}

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

				if (firstBody->GetCollider()->GetIsTrigger() || secondBody->GetCollider()->GetIsTrigger())
				{
					if (firstBody->GetCollider()->GetIsTrigger())
					{
						firstBody->GetCollider()->AddToTriggerList(secondBody->GetCollider());
					}
					if (secondBody->GetCollider()->GetIsTrigger())
					{
						secondBody->GetCollider()->AddToTriggerList(firstBody->GetCollider());
					}
					continue;
				}

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

				/*
				float e = 0.0f;
				Vector3 relativeVelocity = firstBody->GetVelocity() - secondBody->GetVelocity();
				float factor = ( (-(1 + e) * relativeVelocity)).Dot(intersectData.GetDirection() ) /
						  ( intersectData.GetDirection().Dot( intersectData.GetDirection() * (1/firstBody->GetMass() + 1/secondBody->GetMass()) ) );

				Vector3 firstBodyNewVelocity = firstBody->GetVelocity() + ((factor / firstBody->GetMass()) * intersectData.GetDirection());
				Vector3 secondBodyNewVelocity = secondBody->GetVelocity() - ((factor / secondBody->GetMass()) * intersectData.GetDirection());

				firstBody->SetVelocity(firstBodyNewVelocity);
				secondBody->SetVelocity(secondBodyNewVelocity);
				*/

				float e = 0.0f;
				float i1 = 1.0f;
				float i2 = 1.0f;
				Vector3 collisionNormal = intersectData.GetDirection() / intersectData.GetDirection().Length();
				Vector3 relativeVelocity = firstBody->GetVelocity() - secondBody->GetVelocity();
				float factor = ((-(1 + e) * relativeVelocity)).Dot(collisionNormal) /
					((collisionNormal.Dot(collisionNormal * (1 / firstBody->GetMass() + 1 / secondBody->GetMass()))) +
						((intersectData.GetCollisionPointRadiusForFirstBody().Dot(collisionNormal)) * (intersectData.GetCollisionPointRadiusForFirstBody().Dot(collisionNormal)) / i1) +
						((intersectData.GetCollisionPointRadiusForSecondBody().Dot(collisionNormal)) * (intersectData.GetCollisionPointRadiusForSecondBody().Dot(collisionNormal)) / i2));

				Vector3 firstBodyNewVelocity = firstBody->GetVelocity() + ((factor / firstBody->GetMass()) * collisionNormal);
				Vector3 secondBodyNewVelocity = secondBody->GetVelocity() - ((factor / secondBody->GetMass()) * collisionNormal);

				if (i == 1 && j == 2)
				{
					int a = 5;
					a = 6;
				}

				Vector3 firstBodyNewAngularVelocity = firstBody->GetAngularVelocity() + (intersectData.GetCollisionPointRadiusForFirstBody().Cross(factor * collisionNormal) / i1);
				Vector3 secondBodyNewAngularVelocity = secondBody->GetAngularVelocity() + (intersectData.GetCollisionPointRadiusForSecondBody().Cross(-factor * collisionNormal) / i2);

				firstBody->SetVelocity(firstBodyNewVelocity);
				secondBody->SetVelocity(secondBodyNewVelocity);

				//firstBody->SetAngularVelocity(firstBodyNewAngularVelocity);
				//secondBody->SetAngularVelocity(secondBodyNewAngularVelocity);

				if (firstBody->GetIsStatic())
				{
					firstBody->SetVelocity(Vector3(0, 0, 0));
				}
				if (secondBody->GetIsStatic())
				{
					secondBody->SetVelocity(Vector3(0, 0, 0));
				}

				/*if (firstBody->GetIsStatic())
				{
					firstBody->SetAngularVelocity(Vector3(0, 0, 0));
				}
				if (secondBody->GetIsStatic())
				{
					secondBody->SetAngularVelocity(Vector3(0, 0, 0));
				}*/
			}
		}
	}

	for (int i = 0; i < rigidbodies.size(); i++)
	{
		if (rigidbodies[i]->GetCollider()->GetIsTrigger())
		{
			rigidbodies[i]->GetCollider()->UpdateTrigger();
		}
	}
}

void PhysicsEngine::HandleColliderCollisions()
{
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		for (int j = 0; j < colliders.size(); j++)
		{
			if (rigidbodies[i]->GetIsStatic())
			{
				continue;
			}

			Rigidbody* body = rigidbodies[i];
			Collider* collider = colliders[j];

			IntersectData intersectData = body->GetCollider()->Intersect(collider);

			if (intersectData.GetDoesIntersect())
			{
				if (body->GetCollider()->GetIsTrigger() || collider->GetIsTrigger())
				{
					if (body->GetCollider()->GetIsTrigger())
					{
						body->GetCollider()->AddToTriggerList(collider);
					}
					if (collider->GetIsTrigger())
					{
						collider->AddToTriggerList(body->GetCollider());
					}
					continue;
				}

				//
				//
				// SEPARATION
				//
				//

				Vector3 bodyPositionAfterSeparation = body->GetPosition() + intersectData.GetDirection();
				body->SetPosition(bodyPositionAfterSeparation);

				//
				//
				// Combining forces
				//
				//

				Rigidbody* firstBody = body;
				Rigidbody* secondBody = new Rigidbody(nullptr, Vector3(0,0,0), Vector3(0,0,0));

				float e = 0.0f;
				float i1 = 1.0f;
				float i2 = 1.0f;
				Vector3 collisionNormal = intersectData.GetDirection() / intersectData.GetDirection().Length();
				Vector3 relativeVelocity = firstBody->GetVelocity() - secondBody->GetVelocity();
				float factor = ((-(1 + e) * relativeVelocity)).Dot(collisionNormal) /
					((collisionNormal.Dot(collisionNormal * (1 / firstBody->GetMass() + 1 / secondBody->GetMass()))) +
						((intersectData.GetCollisionPointRadiusForFirstBody().Dot(collisionNormal)) * (intersectData.GetCollisionPointRadiusForFirstBody().Dot(collisionNormal)) / i1) +
						((intersectData.GetCollisionPointRadiusForSecondBody().Dot(collisionNormal)) * (intersectData.GetCollisionPointRadiusForSecondBody().Dot(collisionNormal)) / i2));

				Vector3 firstBodyNewVelocity = firstBody->GetVelocity() + ((factor / firstBody->GetMass()) * collisionNormal);
				Vector3 secondBodyNewVelocity = secondBody->GetVelocity() - ((factor / secondBody->GetMass()) * collisionNormal);

				Vector3 firstBodyNewAngularVelocity = firstBody->GetAngularVelocity() + (intersectData.GetCollisionPointRadiusForFirstBody().Cross(factor * collisionNormal) / i1);
				Vector3 secondBodyNewAngularVelocity = secondBody->GetAngularVelocity() + (intersectData.GetCollisionPointRadiusForSecondBody().Cross(-factor * collisionNormal) / i2);

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

				delete secondBody;
			}
		}
	}
}
