#include "LevelGeneratorComp.h"
#include "Components.h"
#include "CMath.h"


LevelGeneratorComp::LevelGeneratorComp(Node* owner, Node* pPlayer, bool rot)
    : Component(owner), pPlayer(pPlayer), spawnNeedRotation(rot)
{

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    points.push_back(Vector4(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z, 0.0f));

}

void LevelGeneratorComp::Update(float dt)
{
    if (isFinished) return;
     
    if (!islandGenerated)
    {
        GenerateIslands();
    }
    if (!enemiesSpawned && islandGenerated) {
        SpawnEnemies();
    }
    if (islandGenerated && enemiesSpawned)
    {
        isFinished = true;
    }
}
void LevelGeneratorComp::GenerateIslands()
{
    if (bigIslandCount <= 0 && mediumIslandCount <= 0 && smallIslandCount <= 0)
    {
        for (int i = 0; i < islands.size(); i++)
        {
            float randY = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 10.0f);
            islands[i]->SetLocalPosition(DirectX::XMFLOAT3(islands[i]->GetLocalPosition().x, islands[i]->GetLocalPosition().y + randY, islands[i]->GetLocalPosition().z));
        }
        islandGenerated = true;
        return;
    }


    int randIsland = rand() % 3;
    if (spawned) {
        spawned = false;
        if (islands.size() > 0)
        {
            while (true) {

                if (randIsland == 0 && bigIslandCount > 0)
                {
                    int randLarge = rand() % 2;
                    if (randLarge == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
                        islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandBig2(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
                        islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    bigIslandCount--;
                    break;
                }
                else if (randIsland == 1 && mediumIslandCount > 0)
                {
                    int randMedium = rand() % 2;
                    if (randMedium == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandMedium1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
                        islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandMedium1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
                        islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    mediumIslandCount--;
                    break;
                }
                else if (randIsland == 2 && smallIslandCount > 0)
                {
                    int randSmall = rand() % 2;
                    if (randSmall == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandSmall1(pOwner, 0.0f, 0.0f, 0.0f, 1.5f);
                        islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);


                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandSmall2(pOwner, 0.0f, 0.0f, 0.0f, 1.5f);
                        islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    smallIslandCount--;
                    break;
                }
                else {
                    randIsland = rand() % 3;
                }
            }
        }
        else {
            //islandPrefab = prefabManager->InstantiateFirstIsland(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
            islandPrefab = PrefabManager::InstantiateFirstIsland(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
        }
        Vector3 p = islandPrefab->GetWorldPosition();
        island = islandPrefab->GetComponent<Island>();
        if(islands.size() != 0)  island->Rotate();
        counterToRotate = 0;

    }
    if (counterToRotate == 3) {
        island->Rotate();
        counterToRotate = 0;
    }
    if (islandPrefab != nullptr && islandPrefab->GetComponent<Island>())
    {

        int randPoint = rand() % points.size();
        Vector4 point = points[randPoint];
        Vector3 pointPos = Vector3(point.x, point.y, point.z);

        if (islands.size() == 0)
        {
            if (spawnNeedRotation) {
                island->Rotate(180);
                ChangePosition(islandPrefab, pointPos, island->upPoint->GetWorldPosition());
                if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                    points.erase(points.begin() + randPoint);
                }

                points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 2.0f));
                points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
            }
            else
            {

                ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
                if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                    points.erase(points.begin() + randPoint);
                }
                points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
            }
            islands.push_back(islandPrefab);
            spawned = true;
        }
        else {
            if (point.w == 0.0f)
            {
                ChangePosition(islandPrefab, pointPos, island->upPoint->GetWorldPosition());
                bool flag = true;
                for (int i = 0; i < islands.size(); i++)
                {
                    if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    //points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                    points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                    points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                    islands.push_back(islandPrefab);
                    spawned = true;
                }

            }
            else if (point.w == 1)
            {
                ChangePosition(islandPrefab, pointPos, island->rightPoint->GetWorldPosition());
                bool flag = true;
                for (int i = 0; i < islands.size(); i++)
                {
                    if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                    //points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                    points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                    islands.push_back(islandPrefab);
                    spawned = true;
                }
            }
            else if (point.w == 2)
            {
                ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
                bool flag = true;
                for (int i = 0; i < islands.size(); i++)
                {
                    if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                    //points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                    points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                    islands.push_back(islandPrefab);
                    spawned = true;
                }

            }
            else if (point.w == 3)
            {
                ChangePosition(islandPrefab, pointPos, island->leftPoint->GetWorldPosition());
                bool flag = true;
                for (int i = 0; i < islands.size(); i++)
                {
                    if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                    points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                    //points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                    islands.push_back(islandPrefab);
                    spawned = true;
                }
            }
            else
            {
                OutputDebugStringA("\nNie prawidlowy punkt\n");
            }

        }
    }
    counterToRotate++;
}

void LevelGeneratorComp::ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos)
{
    Vector3 pos = island->GetWorldPosition();

    Vector3 dir = pointPos - startPos;
    float length = dir.Length();
    dir.Normalize();
    island->SetWorldPosition(island->GetWorldPosition() + (dir * length));
    pos = island->GetWorldPosition();
}

bool LevelGeneratorComp::Collide(Node* island1, Node* island2)
{
    Vector3 pos1 = island1->GetWorldPosition();
    Vector3 pos2 = island2->GetWorldPosition();
    Vector3 extents1 = island1->GetComponent<Island>()->halfExtents;
    Vector3 extents2 = island2->GetComponent<Island>()->halfExtents;

    // Axis-Aligned Bounding Box (AABB) collision check
    if (std::abs(pos1.x - pos2.x) > (extents1.x + extents2.x)) return false;
    if (std::abs(pos1.y - pos2.y) > (extents1.y + extents2.y)) return false;
    if (std::abs(pos1.z - pos2.z) > (extents1.z + extents2.z)) return false;
    return true;
}



void LevelGeneratorComp::SpawnEnemies()
{
    if (numberOfEasyEnemies <= 0 && numberOfMediumEnemies <= 0 && numberOfHardEnemies <= 0)
    {
        OutputDebugStringA("\nAll enemies spawned\n");
        enemiesSpawned = true;
    }

    int randSpot = 0;
    std::vector<Node*> spawnPoints;
    Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
    while (true) // wybor miejsca
    {
        spawnPoints = islands[islandNumber]->GetComponent<Island>()->spawnPoints;
        pIslandNumber = islandNumber;
        if (spawnPoints.size() > 0)
        {
            randSpot = rand() % spawnPoints.size();
            pos = spawnPoints[randSpot]->GetWorldPosition();
            //pos.y = islands[islandNumber]->GetLocalPosition().y;
            //pos.y += spawnPoints[randSpot]->GetLocalPosition().y;

            if (randSpot >= 0 && randSpot < static_cast<int>(spawnPoints.size())) {
                spawnPoints.erase(spawnPoints.begin() + randSpot);
                islands[islandNumber]->GetComponent<Island>()->spawnPoints = spawnPoints;
            }
            islandNumber++;
            break;
        }
        else
        {
            islandNumber++;
            if (islandNumber == pIslandNumber)
            {
                enemiesSpawned = true; // kiedy nie ma wystarczaj¹co miejsc dla enemy
            }
            spawnPoints = islands[islandNumber]->GetComponent<Island>()->spawnPoints;
        }

    }
    if (islandNumber == islands.size())
    {
        islandNumber = 0;
    }

    if (pos != Vector3(0.0f, 0.0f, 0.0f))
    {
        int randEnemy = rand() % 3; // 0 - hard, 1 - medium, 2 - easy
        randEnemy = 0;

        if (randEnemy == 0 && numberOfHardEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                //PrefabManager::InstantiateShootingEnemy(pOwner, pos.x, pos.y, pos.z, 1.6f, pPlayer);
                enemy = PrefabManager::InstantiateShootingEnemy(pOwner, 0.0f, 0.0f, 0.0f, 1.6f, pPlayer);
                enemy->SetWorldPosition(pos);
            }


            numberOfHardEnemies--;
        }
        if (randEnemy == 1 && numberOfMediumEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                //PrefabManager::InstantiateShootingEnemy(pOwner, pos.x, pos.y, pos.z, 1.6f, pPlayer);
            }


            numberOfMediumEnemies--;
        }
        if (randEnemy == 2 && numberOfEasyEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                //PrefabManager::InstantiateShootingEnemy(pOwner, pos.x, pos.y, pos.z, 1.6f, pPlayer);
            }


            numberOfEasyEnemies--;
        }
    }
}

void LevelGeneratorComp::DrawImGuiControls()
{
    ImGui::Checkbox("Island Generated", &islandGenerated);
    ImGui::Checkbox("Enemies Spawned", &enemiesSpawned);
    ImGui::Checkbox("Is Finished", &isFinished);
}
