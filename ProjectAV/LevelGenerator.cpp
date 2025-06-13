#include "LevelGenerator.h"
#include "Components.h"
#include "CMath.h"


LevelGenerator::LevelGenerator(Node* owner, Node* pPlayer, bool rot)
    : Component(owner), pPlayer(pPlayer), spawnNeedRotation(rot)
{
    //points.push_back(Vector4(pOwner->GetWorldPosition().x, pOwner->GetWorldPosition().y, pOwner->GetWorldPosition().z, 0.0f));
    points.push_back(Vector4::Zero);
}

void LevelGenerator::Update(float dt)
{
    if (isFinished) return;

    if (!islandGenerated)
    {
        GenerateIslands();
    }
    if (!enemiesSpawned && islandGenerated && startEnemyGenerating) {
        SpawnEnemies();
    }
    if (islandGenerated && enemiesSpawned)
    {
        isFinished = true;
    }
}
void LevelGenerator::GenerateIslands()
{
    if (bigIslandCount <= 0 && mediumIslandCount <= 0 && smallIslandCount <= 0 && spawned)
    {
        for (int i = 0; i < islandsInfo.size(); i++)
        {
            float randY = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 10.0f);
            islandsInfo[i].pos.y += randY;
            if (islandsInfo[i].name == "BIG1")
            {
                islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
                islandPrefab->SetLocalRotation({ 0.0f, islandsInfo[i].rot, 0.0f });
				islandPrefab->GetComponent<Island>()->leftPoint->SetLocalPosition(islandsInfo[i].leftPoint);
				islandPrefab->GetComponent<Island>()->upPoint->SetLocalPosition(islandsInfo[i].upPoint);
				islandPrefab->GetComponent<Island>()->rightPoint->SetLocalPosition(islandsInfo[i].rightPoint);
				islandPrefab->GetComponent<Island>()->downPoint->SetLocalPosition(islandsInfo[i].downPoint);
            }
            if (islandsInfo[i].name == "FIRST")
            {
                islandPrefab = PrefabManager::InstantiateFirstIsland(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
                islandPrefab->SetLocalRotation({ 0.0f, islandsInfo[i].rot, 0.0f });
                islandPrefab->GetComponent<Island>()->leftPoint->SetLocalPosition(islandsInfo[i].leftPoint);
                islandPrefab->GetComponent<Island>()->upPoint->SetLocalPosition(islandsInfo[i].upPoint);
                islandPrefab->GetComponent<Island>()->rightPoint->SetLocalPosition(islandsInfo[i].rightPoint);
                islandPrefab->GetComponent<Island>()->downPoint->SetLocalPosition(islandsInfo[i].downPoint);
            }
            islands.push_back(islandPrefab);
            
        }
        islandGenerated = true;
        return;
    }


    int randIsland = rand() % 3;
    if (spawned) {
        if (islandsInfo.size() > 0)
        {
            while (true) {

                if (randIsland == 0 && bigIslandCount > 0)
                {
                    spawned = false;
                    int randLarge = rand() % 2;
                    randLarge = 0;
                    if (randLarge == 0) {
                        //islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
                        islandInfo.name = "BIG1";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -65.0f, 0.0f , 35.0f };
                        islandInfo.upPoint = { 40.0f, 0.0f , 65.0f };
                        islandInfo.rightPoint = { 65.0f, 0.0f , 20.0f };
                        islandInfo.downPoint = { 30.0f, 0.0f , -65.0f };
                        islandInfo.extents = {60.0f, 60.0f};
                    }
                    else {
            
                        //islandPrefab = PrefabManager::InstantiateIslandBig2(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    bigIslandCount--;
                    break;
                }
                else if (randIsland == 1 && mediumIslandCount > 0)
                {
                    spawned = false;
                    int randMedium = rand() % 2;
                    if (randMedium == 0) {
                        //islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    else {
                        //islandPrefab = PrefabManager::InstantiateIslandBig1(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    mediumIslandCount--;
                    break;
                }
                else if (randIsland == 2 && smallIslandCount > 0)
                {
                    spawned = false;
                    int randSmall = rand() % 2;
                    if (randSmall == 0) {
                        //islandPrefab = PrefabManager::InstantiateIslandSmall2(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);


                    }
                    else if(randSmall == 1) {
                        //islandPrefab = prefabManager->InstantiateIslandSmall2(pOwner, 0.0f, 0.0f, 0.0f, 1.5f);
                        //islandPrefab = PrefabManager::InstantiateIslandSmall2(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

                    }
                    else {
               
                    //islandPrefab = PrefabManager::InstantiateIslandSmall3(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);

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
            //islandPrefab = PrefabManager::InstantiateFirstIsland(pOwner, 0.0f, 0.0f, 0.0f, 1.0f);
            spawned = false;
            islandInfo.name = "FIRST";
            islandInfo.pos = { 0.0f, 0.0f, 0.0f };
            islandInfo.leftPoint = { -35.0f, 0.0f , 5.0f };
            islandInfo.upPoint = { 3.0f, 0.0f , 36.0f };
            islandInfo.rightPoint = { 35.0f, 0.0f , -4.0f };
            islandInfo.downPoint = { 0.0f, 0.0f , -30.0f };
            islandInfo.extents = { 30.0f, 30.0f };
        }
        if (islandsInfo.size() != 0)  Rotate();
    }
    if (islandInfo.name != "")
    {

        int randPoint = rand() % points.size();
        Vector4 point = points[randPoint];
        Vector3 pointPos = Vector3(point.x, point.y, point.z);
        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
        if (islandsInfo.size() == 0)
        {
            if (spawnNeedRotation) {
                Rotate(180);
                ChangePosition(&islandInfo, pointPos, islandInfo.upPoint);
                if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                    points.erase(points.begin() + randPoint);
                }
                points.push_back(Vector4((islandInfo.leftPoint + islandInfo.pos).x, (islandInfo.leftPoint + islandInfo.pos).y, (islandInfo.leftPoint + islandInfo.pos).z, 1.0f));
                points.push_back(Vector4((islandInfo.downPoint + islandInfo.pos).x, (islandInfo.downPoint + islandInfo.pos).y, (islandInfo.downPoint + islandInfo.pos).z, 2.0f));
                points.push_back(Vector4((islandInfo.rightPoint + islandInfo.pos).x, (islandInfo.rightPoint + islandInfo.pos).y, (islandInfo.rightPoint + islandInfo.pos).z, 3.0f));
            }
            else
            {

                ChangePosition(&islandInfo, pointPos, islandInfo.downPoint);
                if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                    points.erase(points.begin() + randPoint);
                }
                points.push_back(Vector4((islandInfo.leftPoint + islandInfo.pos).x, (islandInfo.leftPoint + islandInfo.pos).y, (islandInfo.leftPoint + islandInfo.pos).z, 1.0f));
                points.push_back(Vector4((islandInfo.upPoint + islandInfo.pos).x, (islandInfo.upPoint + islandInfo.pos).y, (islandInfo.upPoint + islandInfo.pos).z, 2.0f));
                points.push_back(Vector4((islandInfo.rightPoint + islandInfo.pos).x, (islandInfo.rightPoint + islandInfo.pos).y, (islandInfo.rightPoint + islandInfo.pos).z, 3.0f));
            }
            islandsInfo.push_back(islandInfo);
            spawned = true;
        }
        else {
            if (point.w == 0.0f)
            {
                ChangePosition(&islandInfo, pointPos, islandInfo.upPoint);
                bool flag = true;
                for (int i = 0; i < islandsInfo.size(); i++)
                {
                    if (Collide(islandInfo, islandsInfo[i]))
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4((islandInfo.leftPoint + islandInfo.pos).x, (islandInfo.leftPoint + islandInfo.pos).y, (islandInfo.leftPoint + islandInfo.pos).z, 1.0f));
                    points.push_back(Vector4((islandInfo.rightPoint + islandInfo.pos).x, (islandInfo.rightPoint + islandInfo.pos).y, (islandInfo.rightPoint + islandInfo.pos).z, 3.0f));
                    islandsInfo.push_back(islandInfo);
                    spawned = true;
                }

            }
            else if (point.w == 1)
            {
                ChangePosition(&islandInfo, pointPos, islandInfo.rightPoint);
                bool flag = true;
                for (int i = 0; i < islandsInfo.size(); i++)
                {
                    if (Collide(islandInfo, islandsInfo[i]))
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4((islandInfo.downPoint + islandInfo.pos).x, (islandInfo.downPoint + islandInfo.pos).y, (islandInfo.downPoint + islandInfo.pos).z, 0.0f));
                    points.push_back(Vector4((islandInfo.upPoint + islandInfo.pos).x, (islandInfo.upPoint + islandInfo.pos).y, (islandInfo.upPoint + islandInfo.pos).z, 2.0f));
                    islandsInfo.push_back(islandInfo);
                    spawned = true;
                }
            }
            else if (point.w == 2)
            {
                ChangePosition(&islandInfo, pointPos, islandInfo.downPoint);
                bool flag = true;
                for (int i = 0; i < islandsInfo.size(); i++)
                {
                    if (Collide(islandInfo, islandsInfo[i]))
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4((islandInfo.leftPoint + islandInfo.pos).x, (islandInfo.leftPoint + islandInfo.pos).y, (islandInfo.leftPoint + islandInfo.pos).z, 1.0f));
                    points.push_back(Vector4((islandInfo.rightPoint + islandInfo.pos).x, (islandInfo.rightPoint + islandInfo.pos).y, (islandInfo.rightPoint + islandInfo.pos).z, 3.0f));
                    islandsInfo.push_back(islandInfo);
                    spawned = true;
                }

            }
            else if (point.w == 3)
            {
                ChangePosition(&islandInfo, pointPos, islandInfo.leftPoint);
                bool flag = true;
                for (int i = 0; i < islandsInfo.size(); i++)
                {
                    if (Collide(islandInfo, islandsInfo[i]))
                    {
                        flag = false;
                    }

                }
                if (flag)
                {
                    if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                        points.erase(points.begin() + randPoint);
                    }
                    points.push_back(Vector4((islandInfo.downPoint + islandInfo.pos).x, (islandInfo.downPoint + islandInfo.pos).y, (islandInfo.downPoint + islandInfo.pos).z, 0.0f));
                    points.push_back(Vector4((islandInfo.upPoint + islandInfo.pos).x, (islandInfo.upPoint + islandInfo.pos).y, (islandInfo.upPoint + islandInfo.pos).z, 2.0f));
                    islandsInfo.push_back(islandInfo);
                    spawned = true;
                }
            }
            else
            {
                OutputDebugStringA("\nNie prawidlowy punkt\n");
            }

        }
    }
    }

    void LevelGenerator::ChangePosition(islandInfoStruct* island, Vector3 pointPos, Vector3 startPos)
    {
        Vector3 pos = startPos;
        pos += island->pos;
        Vector3 dir = pointPos - pos;
        float length = dir.Length();
        dir.Normalize();
        island->pos += (dir * length);
    }

bool LevelGenerator::Collide(islandInfoStruct island1, islandInfoStruct island2)
{

    if (std::abs(island1.pos.x - island2.pos.x) > (island1.extents.x + island2.extents.x)) return false;
    if (std::abs(island1.pos.z - island2.pos.z) > (island1.extents.y + island2.extents.y)) return false;
    return true;
}



void LevelGenerator::SpawnEnemies()
{
    if (numberOfEasyEnemies <= 0 && numberOfMediumEnemies <= 0 && numberOfHardEnemies <= 0)
    {
        OutputDebugStringA("\nAll enemies spawned\n");
        enemiesSpawned = true;
        return;
    }

    int randSpot = 0;
    std::vector<Node*> spawnPoints;
    Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
    pIslandNumber = islandNumber;
    while (true)
    {
        spawnPoints.clear();
        spawnPoints = islands[islandNumber]->GetComponent<Island>()->spawnPoints;
        if (spawnPoints.size() > 0)
        {
            randSpot = rand() % spawnPoints.size();
            pos = spawnPoints[randSpot]->GetWorldPosition();

            if (randSpot >= 0 && randSpot < static_cast<int>(spawnPoints.size())) {
                spawnPoints.erase(spawnPoints.begin() + randSpot);
                islands[islandNumber]->GetComponent<Island>()->spawnPoints = spawnPoints;
            }
            islandNumber++;
            if (islandNumber >= islandsInfo.size())
            {
                islandNumber = 0;
            }
            break;
        }
        else
        {
            islandNumber++;
            if (islandNumber >= islandsInfo.size())
            {
                islandNumber = 0;
            }
        }
        if (islandNumber == pIslandNumber)
        {
            //enemiesSpawned = true;
            break;
        }

    }

    if (pos != Vector3(0.0f, 0.0f, 0.0f))
    {
        int randEnemy = rand() % 3;

        if (randEnemy == 0 && numberOfHardEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                enemy = PrefabManager::InstantiateFlyingEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f), 1.6f, pPlayer);
                enemy->SetWorldPosition(pos);
            }


            numberOfHardEnemies--;
        }
        if (randEnemy == 1 && numberOfMediumEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                enemy = PrefabManager::InstantiateShootingEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f), 1.6f, pPlayer);
                enemy->SetWorldPosition(pos);
            }


            numberOfMediumEnemies--;
        }
        if (randEnemy == 2 && numberOfEasyEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                enemy = PrefabManager::InstantiateNormalEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f), 1.6f, pPlayer);
                enemy->SetWorldPosition(pos);
            }


            numberOfEasyEnemies--;
        }
    }
}


void LevelGenerator::Rotate(int rot)
{
    int randomRotate = rand() % 4;
    randomRotate = randomRotate * 90;
    if (rot != 0) randomRotate = rot;
    islandInfo.rot = to_rad(randomRotate);
    if (randomRotate == 90)
    {
		islandInfo.upPoint = { islandInfo.upPoint.z, islandInfo.upPoint.y, -islandInfo.upPoint.x };
		islandInfo.downPoint = { islandInfo.downPoint.z, islandInfo.downPoint.y, -islandInfo.downPoint.x };
		islandInfo.leftPoint = { islandInfo.leftPoint.z, islandInfo.leftPoint.y, -islandInfo.leftPoint.x };
		islandInfo.rightPoint = { islandInfo.rightPoint.z, islandInfo.rightPoint.y, -islandInfo.rightPoint.x };

        Vector3 pos = islandInfo.upPoint;
        islandInfo.upPoint = islandInfo.leftPoint;
        islandInfo.leftPoint = islandInfo.downPoint;
        islandInfo.downPoint = islandInfo.rightPoint;
        islandInfo.rightPoint = pos;
    }
    else if (randomRotate == 180)
    {
        islandInfo.upPoint = { -islandInfo.upPoint.x, islandInfo.upPoint.y, -islandInfo.upPoint.z };
		islandInfo.downPoint = { -islandInfo.downPoint.x, islandInfo.downPoint.y, -islandInfo.downPoint.z };
		islandInfo.leftPoint = { -islandInfo.leftPoint.x, islandInfo.leftPoint.y, -islandInfo.leftPoint.z };
		islandInfo.rightPoint = { -islandInfo.rightPoint.x, islandInfo.rightPoint.y, -islandInfo.rightPoint.z };

        Vector3 pos = islandInfo.upPoint;
        islandInfo.upPoint = islandInfo.downPoint;
        islandInfo.downPoint = pos;
        pos = islandInfo.leftPoint;
        islandInfo.leftPoint = islandInfo.rightPoint;
        islandInfo.rightPoint = pos;
    }
    else if (randomRotate == 270)
    {
        islandInfo.upPoint = { -islandInfo.upPoint.z, islandInfo.upPoint.y, islandInfo.upPoint.x };
		islandInfo.downPoint = { -islandInfo.downPoint.z, islandInfo.downPoint.y, islandInfo.downPoint.x };
		islandInfo.leftPoint = { -islandInfo.leftPoint.z, islandInfo.leftPoint.y, islandInfo.leftPoint.x };
		islandInfo.rightPoint = { -islandInfo.rightPoint.z, islandInfo.rightPoint.y, islandInfo.rightPoint.x };

        Vector3 pos = islandInfo.upPoint;
        islandInfo.upPoint = islandInfo.rightPoint;
        islandInfo.rightPoint = islandInfo.downPoint;
        islandInfo.downPoint = islandInfo.leftPoint;
        islandInfo.leftPoint = pos;
    }

}


void LevelGenerator::DrawImGuiControls()
{
    ImGui::Checkbox("Island Generated", &islandGenerated);
    ImGui::Checkbox("Enemies Spawned", &enemiesSpawned);
    ImGui::Checkbox("Is Finished", &isFinished);
	ImGui::Checkbox("Start Enemy Generating", &startEnemyGenerating);
}
