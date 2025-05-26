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
            OutputDebugStringA("\nNie uda�o si�\n");
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
			if (points[randPoint].w == lastPointW)
			{
				randPoint = rand() % points.size();
			}
            lastPointW = points[randPoint].w;
			//OutputDebugStringA(("\nRandPoint: " + std::to_string(randPoint) + "Point w: " + std::to_string(points[randPoint].w)).c_str());
			Vector4 point = points[randPoint];
            Vector3 pointPos = Vector3(point.x, point.y, point.z);
            islandPrefab->SetWorldPosition(pointPos);
            if (pointPos == Vector3(0.0f,0.0f,0.0f))
            {
                while (true)
                {
                    if (time > 10.0f)
                    {
						OutputDebugStringA("\nNie uda�o si� znale�� miejsca dla wyspy pierwszej\n");
                        time = 0.0f;
                        break;
                    }
					time += 0.01f;
					ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
					OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str() );
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
					points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                    points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                    time = 0.0f;

                    //doda� warunek sprawdzenia czy nie wchodzi w kolizje
                    break;
                }
            }
            else {
                //island->Rotate();
                while (true)
                {
                    time += 0.01f;
                    if (time > 25.0f)
                    {
						OutputDebugStringA("\nNie uda�o si� znale�� miejsca dla wyspy\n");
                        time = 0.0f;
                        break;
                    }
                    if (point.w == 0.0f)
                    {
                        ChangePosition(islandPrefab, pointPos, island->upPoint->GetWorldPosition());
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                      
                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                        points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                        points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        time = 0.0f;
                        break;
                    }
                    else if (point.w == 1)
                    {
                        ChangePosition(islandPrefab, pointPos, island->rightPoint->GetWorldPosition());
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());

                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                        points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                        points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                        time = 0.0f;
                        break;
                    }
                    else if (point.w == 2)
                    {
                        ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());

                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                        points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                        points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        time = 0.0f;
                        break;
                    }
                    else if (point.w == 3)
                    {
                        ChangePosition(islandPrefab, pointPos, island->upPoint->GetWorldPosition());
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());

                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                        points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                        points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        time = 0.0f;
						break;
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

void LevelGenerator::ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos)
{
	OutputDebugStringA(("\n\nChanging position of island: " + island->GetName()).c_str());
	OutputDebugStringA(("\nStart Position: " + std::to_string(startPos.x) + ", " + std::to_string(startPos.y) + ", " + std::to_string(startPos.z)).c_str());
	OutputDebugStringA(("\nPoint Position: " + std::to_string(pointPos.x) + ", " + std::to_string(pointPos.y) + ", " + std::to_string(pointPos.z)).c_str());
    Vector3 pos = island->GetWorldPosition();
    OutputDebugStringA(("\nIsland Position: " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z)).c_str());

	Vector3 dir = pointPos - startPos;
	float length = dir.Length();
	dir.Normalize();
	island->SetWorldPosition(island->GetWorldPosition() + (dir * length));
    pos = island->GetWorldPosition();
	OutputDebugStringA(("\nIsland New Position: " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z)).c_str());

    PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(island->GetComponent<Rigidbody>()->GetBodyID(), Vec3(pos.x, pos.y, pos.z), EActivation::Activate);
}
