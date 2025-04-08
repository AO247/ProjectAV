#include "PhysicsEngine.h"
#include <sstream>

void PhysicsEngine::AddRigidbody(std::shared_ptr<Rigidbody> rigidbody)
{
	rigidbodies.push_back(rigidbody);
}

void PhysicsEngine::Simulate(float delta)
{
	for (int i = 0; i < rigidbodies.size(); i++)
	{
		rigidbodies[i]->Integrate(delta);
	}
	//for (Rigidbody* rigidbody : rigidbodies)
	//{
	//	rigidbody->Integrate(delta);
	//}
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
				OutputDebugString("JEST KOLIZJA \n");
				std::ostringstream ss3;
				ss3 << (intersectData.GetDirection() * -1).x;
				std::string s3(ss3.str());
				s3 += " X SEPARACJI\n";
				OutputDebugString(s3.c_str());
				Vector3 firstBodyPositionAfterSeparation = rigidbodies[i]->GetPosition() + (intersectData.GetDirection());
				rigidbodies[i]->SetPosition(firstBodyPositionAfterSeparation);
			}
		}
	}
}