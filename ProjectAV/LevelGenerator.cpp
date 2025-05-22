#include "LevelGenerator.h"
#include "Components.h"

LevelGenerator::LevelGenerator(PrefabManager* prefabManager, Node* root)
	: prefabManager(prefabManager), pSceneRoot(root)
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));

	islands.push_back("large_1");
	points.push_back(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
    GenerateIslands();
}

void LevelGenerator::GenerateIslands()
{
	float time = 0.0f;
	int lastPointW = 1;
    while (true)
    {

        if (time > 15.0f)
        {
            OutputDebugStringA("\nNie uda³o siê\n");
            time = 0.0f;
            break;
        }
        int randIsland = rand() % 3;
		Node* islandPrefab = nullptr;

        while (true) {

            if (randIsland == 0 && bigIslandCount > 0)
            {
                islandPrefab = prefabManager->InstantiateIslandBig1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.3f);
				bigIslandCount--;
                break;
            }
            else if (randIsland == 1 && mediumIslandCount > 0)
            {
				int randMedium = rand() % 2;
                if (randMedium == 0) {
                    islandPrefab = prefabManager->InstantiateIslandMedium1(pSceneRoot, 0.0f, 0.0f, 0.0f, 2.3f);
                }
                else {
					islandPrefab = prefabManager->InstantiateIslandMedium2(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.3f);
                }
				mediumIslandCount--;
                break;
            }
            else if (randIsland == 2 && smallIslandCount > 0)
            {
                islandPrefab = prefabManager->InstantiateIslandSmall1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.5f);
				smallIslandCount--;
                break;
            }
            else {
				randIsland = rand() % 3;
            }
        }

        if (islandPrefab != nullptr && islandPrefab->GetComponent<Island>())
        {
            Island* island = islandPrefab->GetComponent<Island>();

            int randPoint = rand() % points.size();
			if (randPoint == lastPointW)
			{
				randPoint = (randPoint + 1) % points.size();
			}
			lastPointW = randPoint;
			//OutputDebugStringA(("\nRandPoint: " + std::to_string(randPoint) + "Point w: " + std::to_string(points[randPoint].w)).c_str());
			Vector4 point = points[randPoint];
            Vector3 pos = Vector3(point.x, point.y, point.z);
            islandPrefab->SetWorldPosition(pos);
            if (pos == Vector3(0.0f,0.0f,0.0f))
            {
                while (true)
                {
                    if (time > 10.0f)
                    {
						OutputDebugStringA("\nNie uda³o siê znaleŸæ miejsca dla wyspy pierwszej\n");
                        time = 0.0f;
                        break;
                    }
					time += 0.01f;
                    islandPrefab->SetWorldPosition(Vector3(islandPrefab->GetWorldPosition().x, islandPrefab->GetWorldPosition().y, islandPrefab->GetWorldPosition().z + 1.0f));
                    if ((island->downPoint->GetWorldPosition() - pos).Length() <= distance)
                    {
						OutputDebugStringA("\nZnaleziono miejsce dla wyspy pierwszej\n");
                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
						points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
						points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        
                        //placedIslands.Add(islandPrefab);

                        /*while (!Physics.Raycast(player.transform.position, Vector3.down, player.GetComponent<FPPlayerMovement>().playerHeight * 0.5f, player.GetComponent<FPPlayerMovement>().whatIsGround))
                        {
                            player.transform.position = new Vector3(player.transform.position.x, player.transform.position.y - 0.5f, player.transform.position.z);
                            if (player.transform.position.y <= 0.0f)
                            {
                                break;
                            }
                        }*/
                        time = 0.0f;
                        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(islandPrefab->GetComponent<Rigidbody>()->GetBodyID(), Vec3(pos.x, pos.y, pos.z), EActivation::Activate);
                        break;
                    }
                }
            }
            else {
                island->Rotate();
                float stepSize = 1.0f;
                bool calculated = false;
                Vector3 currentPos = Vector3(0.0f, 0.0f, 0.0f);
                Vector3 targetPos = Vector3(0.0f, 0.0f, 0.0f);
                Vector3 dir = Vector3(0.0f, 0.0f, 0.0f);
                //OutputDebugStringA(("\nPozycja point: " + std::to_string(point.w)).c_str());

                while (true)
                {
                    time += 0.01f;
                    if (time > 25.0f)
                    {
						OutputDebugStringA("\nNie uda³o siê znaleŸæ miejsca dla wyspy\n");
                        time = 0.0f;
                        break;
                    }
                    if (point.w == 0.0f)
                    {
                        if (!calculated)
                        {
                            calculated = true;
                            currentPos = islandPrefab->GetWorldPosition();
                            targetPos = island->upPoint->GetWorldPosition();
                            dir = targetPos - currentPos;
                            dir.Normalize();
                        }
                        currentPos = islandPrefab->GetWorldPosition();
                        islandPrefab->SetWorldPosition(currentPos + -dir * stepSize);
                        // sprawdŸ, czy ju¿ jesteœ blisko:
                        if ((island->upPoint->GetWorldPosition() - pos).Length() <= distance)
                        {
                            OutputDebugStringA("\nZnaleziono miejsce dla wyspy\n");
                            if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                                points.erase(points.begin() + randPoint);
                            }
                            points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                            points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                            points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                            time = 0.0f;
                            PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(islandPrefab->GetComponent<Rigidbody>()->GetBodyID(), Vec3(pos.x, pos.y, pos.z), EActivation::Activate);
                            break;

                        }
                    }
                    else if (point.w == 1)
                    {
                        if (!calculated)
                        {
                            calculated = true;
                            currentPos = islandPrefab->GetWorldPosition();
                            targetPos = island->rightPoint->GetWorldPosition();
                            dir = targetPos - currentPos;
                            dir.Normalize();
                        }
                        currentPos = islandPrefab->GetWorldPosition();
                        islandPrefab->SetWorldPosition(currentPos + -dir * stepSize);
                        // sprawdŸ, czy ju¿ jesteœ blisko:
                        if ((island->rightPoint->GetWorldPosition() - pos).Length() <= distance)
                        {
                            OutputDebugStringA("\nZnaleziono miejsce dla wyspy\n");
                            if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                                points.erase(points.begin() + randPoint);
                            }
                            points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                            points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                            points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                            time = 0.0f;
                            PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(islandPrefab->GetComponent<Rigidbody>()->GetBodyID(), Vec3(pos.x, pos.y, pos.z), EActivation::Activate);
                            break;

                        }
                    }
                    else if (point.w == 2)
                    {

                        if (!calculated)
                        {
                            calculated = true;
                            currentPos = islandPrefab->GetWorldPosition();
                            targetPos = island->downPoint->GetWorldPosition();
                            dir = targetPos - currentPos;
                            dir.Normalize();
                        }
                        currentPos = islandPrefab->GetWorldPosition();
                        islandPrefab->SetWorldPosition(currentPos + -dir * stepSize);

                        // sprawdŸ, czy ju¿ jesteœ blisko:
                        if ((island->downPoint->GetWorldPosition() - pos).Length() <= distance)
                        {
                            OutputDebugStringA("\nZnaleziono miejsce dla wyspy\n");
                            if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                                points.erase(points.begin() + randPoint);
                            }
                            points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                            points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                            points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                            time = 0.0f;
                            PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(islandPrefab->GetComponent<Rigidbody>()->GetBodyID(), Vec3(pos.x, pos.y, pos.z), EActivation::Activate);
                            break;

                        }
                    }
                    else if (point.w == 3)
                    {
                        if (!calculated)
                        {
                            calculated = true;
                            currentPos = islandPrefab->GetWorldPosition();
                            targetPos = island->leftPoint->GetWorldPosition();
                            dir = targetPos - currentPos;
                            dir.Normalize();
                        }
                        currentPos = islandPrefab->GetWorldPosition();
                        islandPrefab->SetWorldPosition(currentPos + -dir * stepSize);

                        // sprawdŸ, czy ju¿ jesteœ blisko:
                        if ((island->leftPoint->GetWorldPosition() - pos).Length() <= distance)
                        {
                            OutputDebugStringA("\nZnaleziono miejsce dla wyspy\n");
                            if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                                points.erase(points.begin() + randPoint);
                            }
                            points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                            points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                            points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                            time = 0.0f;
							Vector3 pos = Vector3(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z);
							PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(islandPrefab->GetComponent<Rigidbody>()->GetBodyID(), Vec3(pos.x, pos.y, pos.z), EActivation::Activate);
                            break;
                        }
                    }
                    else
                    {
						OutputDebugStringA("\nNie prawidlowy punkt\n");
                    }
                }
            }
        }
        if (bigIslandCount <= 0 && mediumIslandCount <= 0 && smallIslandCount <= 0)
        {
			OutputDebugStringA("\nAll islands generated\n");
            time = 0.0f;
            break;
        }
    }
}