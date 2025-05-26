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
            if (points[randPoint].w == lastPointW)
            {
                randPoint = rand() % points.size();
            }
            lastPointW = points[randPoint].w;
            //OutputDebugStringA(("\nRandPoint: " + std::to_string(randPoint) + "Point w: " + std::to_string(points[randPoint].w)).c_str());
            Vector4 point = points[randPoint];
            Vector3 pointPos = Vector3(point.x, point.y, point.z);
            islandPrefab->SetWorldPosition(pointPos);
            if (pointPos == Vector3(0.0f, 0.0f, 0.0f))
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
                    ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
                    OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                    points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                    points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                    time = 0.0f;

                    //dodaæ warunek sprawdzenia czy nie wchodzi w kolizje
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
                        OutputDebugStringA("\nNie uda³o siê znaleŸæ miejsca dla wyspy\n");
                        time = 0.0f;
                        break;
                    }
                    if (point.w == 0.0f)
                    {
                        ChangePosition(islandPrefab, pointPos, island->upPoint->GetWorldPosition());
                        Vec3 jPos = Vec3(islandPrefab->GetWorldPosition().x, islandPrefab->GetWorldPosition().y, islandPrefab->GetWorldPosition().z);
                        Vec3 jExtents = Vec3(1.0f, 2.0f, 1.f);
                        Quat quat = Quat(Vec4Arg(islandPrefab->GetLocalRotationQuaternion().x, islandPrefab->GetLocalRotationQuaternion().y, islandPrefab->GetLocalRotationQuaternion().z, islandPrefab->GetLocalRotationQuaternion().w));
                        BodyIDVector bodies = PhysicsCommon::OverlapBox(jPos, jExtents, quat);
                        bool flag = true;
                        for (int i = 0; i < bodies.size(); i++)
                        {
                            if (PhysicsCommon::physicsSystem->GetBodyInterface().GetObjectLayer(bodies[i]) == Layers::GROUND && bodies[i] != islandPrefab->GetComponent<Rigidbody>()->GetBodyID())
                            {
                                OutputDebugStringA(("\nCollides with: \n" + std::to_string(bodies[i].GetIndex()) + " current body: " + std::to_string(islandPrefab->GetComponent<Rigidbody>()->GetBodyID().GetIndex())).c_str());

                                flag = false;
                            }
                        }
                        /*if (!flag)
                        {
                            OutputDebugStringA("\nCollides\n");
                            randPoint = rand() % points.size();
                            if (points[randPoint].w == lastPointW)
                            {
                                randPoint = rand() % points.size();
                            }
                            lastPointW = points[randPoint].w;
                            point = points[randPoint];
                            pointPos = Vector3(point.x, point.y, point.z);
                            continue;
                        }*/
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt gorny z dolem\n");
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
                        Vec3 jPos = Vec3(islandPrefab->GetWorldPosition().x, islandPrefab->GetWorldPosition().y, islandPrefab->GetWorldPosition().z);
                        Vec3 jExtents = Vec3(island->halfExtents.x, island->halfExtents.y, island->halfExtents.z);
                        Quat quat = Quat(Vec4Arg(islandPrefab->GetLocalRotationQuaternion().x, islandPrefab->GetLocalRotationQuaternion().y, islandPrefab->GetLocalRotationQuaternion().z, islandPrefab->GetLocalRotationQuaternion().w));
                        BodyIDVector bodies = PhysicsCommon::OverlapBox(jPos, jExtents, quat);
                        bool flag = true;
                        for (int i = 0; i < bodies.size(); i++)
                        {
                            if (PhysicsCommon::physicsSystem->GetBodyInterface().GetObjectLayer(bodies[i]) == Layers::GROUND && bodies[i] != islandPrefab->GetComponent<Rigidbody>()->GetBodyID())
                            {
                                OutputDebugStringA(("\nCollides with: \n" + std::to_string(bodies[i].GetIndex()) + " current body: " + std::to_string(islandPrefab->GetComponent<Rigidbody>()->GetBodyID().GetIndex())).c_str());

                                flag = false;
                            }
                        }
                        /*if (!flag)
                        {
                            OutputDebugStringA("\nCollides\n");
                            randPoint = rand() % points.size();
                            if (points[randPoint].w == lastPointW)
                            {
                                randPoint = rand() % points.size();
                            }
                            lastPointW = points[randPoint].w;
                            point = points[randPoint];
                            pointPos = Vector3(point.x, point.y, point.z);
                            continue;
                        }*/
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt prawy z lewym\n");

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
                        Vec3 jPos = Vec3(islandPrefab->GetWorldPosition().x, islandPrefab->GetWorldPosition().y, islandPrefab->GetWorldPosition().z);
                        Vec3 jExtents = Vec3(island->halfExtents.x, island->halfExtents.y, island->halfExtents.z);
                        Quat quat = Quat(Vec4Arg(islandPrefab->GetLocalRotationQuaternion().x, islandPrefab->GetLocalRotationQuaternion().y, islandPrefab->GetLocalRotationQuaternion().z, islandPrefab->GetLocalRotationQuaternion().w));
                        BodyIDVector bodies = PhysicsCommon::OverlapBox(jPos, jExtents, quat);
                        bool flag = true;
                        for (int i = 0; i < bodies.size(); i++)
                        {
                            if (PhysicsCommon::physicsSystem->GetBodyInterface().GetObjectLayer(bodies[i]) == Layers::GROUND && bodies[i] != islandPrefab->GetComponent<Rigidbody>()->GetBodyID())
                            {
                                OutputDebugStringA(("\nCollides with: \n" + std::to_string(bodies[i].GetIndex()) + " current body: " + std::to_string(islandPrefab->GetComponent<Rigidbody>()->GetBodyID().GetIndex())).c_str());

                                flag = false;
                            }
                        }
                        /*if (!flag)
                        {
                            OutputDebugStringA("\nCollides\n");
                            randPoint = rand() % points.size();
                            if (points[randPoint].w == lastPointW)
                            {
                                randPoint = rand() % points.size();
                            }
                            lastPointW = points[randPoint].w;
                            point = points[randPoint];
                            pointPos = Vector3(point.x, point.y, point.z);
                            continue;
                        }*/
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt dolny z gornym\n");

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
                        ChangePosition(islandPrefab, pointPos, island->leftPoint->GetWorldPosition());
                        Vec3 jPos = Vec3(islandPrefab->GetWorldPosition().x, islandPrefab->GetWorldPosition().y, islandPrefab->GetWorldPosition().z);
                        Vec3 jExtents = Vec3(island->halfExtents.x, island->halfExtents.y, island->halfExtents.z);
                        Quat quat = Quat(Vec4Arg(islandPrefab->GetLocalRotationQuaternion().x, islandPrefab->GetLocalRotationQuaternion().y, islandPrefab->GetLocalRotationQuaternion().z, islandPrefab->GetLocalRotationQuaternion().w));
                        BodyIDVector bodies = PhysicsCommon::OverlapBox(jPos, jExtents, quat);
                        bool flag = true;
                        for (int i = 0; i < bodies.size(); i++)
                        {
                            if (PhysicsCommon::physicsSystem->GetBodyInterface().GetObjectLayer(bodies[i]) == Layers::GROUND && bodies[i] != islandPrefab->GetComponent<Rigidbody>()->GetBodyID())
                            {
                                OutputDebugStringA(("\nCollides with: \n" + std::to_string(bodies[i].GetIndex()) + " current body: " + std::to_string(islandPrefab->GetComponent<Rigidbody>()->GetBodyID().GetIndex())).c_str());
                                flag = false;
                            }
                        }
                        /*if (!flag)
                        {
                            OutputDebugStringA("\nCollides\n");
                            randPoint = rand() % points.size();
                            if (points[randPoint].w == lastPointW)
                            {
                                randPoint = rand() % points.size();
                            }
                            lastPointW = points[randPoint].w;
                            point = points[randPoint];
                            pointPos = Vector3(point.x, point.y, point.z);
                            continue;
                        }*/
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt lewy z prawym\n");

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
    OutputDebugStringA(("\nPoint Position: " + std::to_string(pointPos.x) + ", " + std::to_string(pointPos.y) + ", " + std::to_string(pointPos.z)).c_str());
    Vector3 pos = island->GetWorldPosition();

    Vector3 dir = pointPos - startPos;
    float length = dir.Length();
    dir.Normalize();
    island->SetWorldPosition(island->GetWorldPosition() + (dir * length));
    pos = island->GetWorldPosition();
    OutputDebugStringA(("\nIsland New Position: " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z)).c_str());

}
