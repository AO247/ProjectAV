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
		islandLimitTimer -= dt;
        if(islandLimitTimer <= 0.0f) {
			bigIslandCount = 0;
			mediumIslandCount = 0;
			smallIslandCount = 0;
            spawned = true;
		}
    }
    if (!enemiesSpawned && islandGenerated && startEnemyGenerating) {
        SpawnEnemies();
        enemiesLimitTimer -= dt;
        if (enemiesLimitTimer <= 0.0f) enemiesSpawned = true;
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

			}
            else if (islandsInfo[i].name == "BIG2")
            {
                islandPrefab = PrefabManager::InstantiateIslandBig2(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
            }
			else if (islandsInfo[i].name == "BIG3")
			{
				islandPrefab = PrefabManager::InstantiateIslandBig3(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "BIG4")
			{
				islandPrefab = PrefabManager::InstantiateIslandBig4(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
            else if (islandsInfo[i].name == "BIG5")
            {
                islandPrefab = PrefabManager::InstantiateIslandBig5(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
            }
			else if (islandsInfo[i].name == "BIG6")
			{
				islandPrefab = PrefabManager::InstantiateIslandBig6(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "BIG7")
			{
				islandPrefab = PrefabManager::InstantiateIslandBig7(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "BIG8")
			{
				islandPrefab = PrefabManager::InstantiateIslandBig8(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "BIG9")
			{
				islandPrefab = PrefabManager::InstantiateIslandBig9(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM1")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium1(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM2")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium2(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM3")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium3(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM4")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium4(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM5")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium5(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM6")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium6(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM7")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium7(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM8")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium8(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "MEDIUM9")
			{
				islandPrefab = PrefabManager::InstantiateIslandMedium9(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL1")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall1(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL2")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall2(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL3")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall3(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL4")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall4(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL5")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall5(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL6")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall6(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL7")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall7(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL8")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall8(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL9")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall9(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL10")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall10(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL11")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall11(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
			else if (islandsInfo[i].name == "SMALL12")
			{
				islandPrefab = PrefabManager::InstantiateIslandSmall12(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
			}
            if (islandsInfo[i].name == "FIRST")
            {
                islandPrefab = PrefabManager::InstantiateFirstIsland(pOwner, Vector3(islandsInfo[i].pos.x, islandsInfo[i].pos.y, islandsInfo[i].pos.z), 1.0f);
            }
            islandPrefab->SetLocalRotation({ 0.0f, islandsInfo[i].rot, 0.0f });
            islandPrefab->GetComponent<Island>()->leftPoint->SetLocalPosition(islandsInfo[i].leftPoint);
            islandPrefab->GetComponent<Island>()->upPoint->SetLocalPosition(islandsInfo[i].upPoint);
            islandPrefab->GetComponent<Island>()->rightPoint->SetLocalPosition(islandsInfo[i].rightPoint);
            islandPrefab->GetComponent<Island>()->downPoint->SetLocalPosition(islandsInfo[i].downPoint);
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
                    int randLarge = rand() % 9;
                    if (randLarge == 0) {
                        islandInfo.name = "BIG1";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -53.90f, 5.20f, -1.30f };
                        islandInfo.upPoint = { -10.00f, 0.00f, 60.00f };
                        islandInfo.rightPoint = { 66.00f, 0.00f, 15.00f };
                        islandInfo.downPoint = { -2.20f, 0.00f, -55.30f };
                    }
                    else if(randLarge == 1){
                        islandInfo.name = "BIG2";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -56.00f, 3.00f, -9.90f };
                        islandInfo.upPoint = { -7.00f, 10.50f, 60.70f };
                        islandInfo.rightPoint = { 67.00f, 0.00f, 6.00f };
                        islandInfo.downPoint = { 4.00f, 0.00f, -58.00f };
                    }
                    else if (randLarge == 2) {
						islandInfo.name = "BIG3";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -58.50f, 18.00f, 4.30f };
						islandInfo.upPoint = { 47.70f, 0.00f, 48.00f };
						islandInfo.rightPoint = { 81.00f, 0.00f, -4.00f };
						islandInfo.downPoint = { 0.00f, 0.00f, -42.60f };
                    }
                    else if (randLarge == 3) {
						islandInfo.name = "BIG4";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -56.40f, 20.00f, 4.30f };
						islandInfo.upPoint = { 9.80f, 0.00f, 50.00f };
						islandInfo.rightPoint = { 82.00f, 0.00f, -4.00f };
						islandInfo.downPoint = { -5.10f, 0.00f, -42.20f };
                    }
                    else if (randLarge == 4) {
                        islandInfo.name = "BIG5";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -57.30f, 20.00f, 4.30f };
                        islandInfo.upPoint = { 55.20f, 0.00f, 44.70f };
                        islandInfo.rightPoint = { 82.00f, 0.00f, -4.00f };
                        islandInfo.downPoint = { -4.90f, 0.00f, -42.00f };
                    }
                    else if (randLarge == 5) {
						islandInfo.name = "BIG6";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -61.00f, 5.00f, 4.30f };
						islandInfo.upPoint = { 8.60f, 5.00f, 49.30f };
						islandInfo.rightPoint = { 81.90f, 0.00f, 7.80f };
						islandInfo.downPoint = { -5.90f, 0.00f, -41.70f };
                    }
                    else if (randLarge == 6) {
						islandInfo.name = "BIG7";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -54.60f, 4.30f, 11.10f };
						islandInfo.upPoint = { -7.00f, 0.00f, 64.00f };
						islandInfo.rightPoint = { 67.00f, 0.00f, -12.00f };
						islandInfo.downPoint = { -7.50f, 0.00f, -56.70f };
                    }
                    else if (randLarge == 7) {
						islandInfo.name = "BIG8";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -53.70f, 0.00f, -2.60f };
						islandInfo.upPoint = { -7.00f, 0.00f, 61.70f };
						islandInfo.rightPoint = { 67.00f, 0.00f, -12.00f };
						islandInfo.downPoint = { 12.00f, 3.40f, -55.00f };
                    }
                    else if(randLarge == 8) {
                        islandInfo.name = "BIG9";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -52.10f, 0.00f, -2.10f };
                        islandInfo.upPoint = { -7.60f, -3.00f, 62.30f };
                        islandInfo.rightPoint = { 67.00f, 0.00f, -19.90f };
                        islandInfo.downPoint = { -15.70f, 0.00f, -56.50f };
                    } 
                    bigIslandCount--;
                    break;
                }
                else if (randIsland == 1 && mediumIslandCount > 0)
                {
                    spawned = false;
                    int randMedium = rand() % 9;
                    if (randMedium == 0) {
                        islandInfo.name = "MEDIUM1";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -44.00f, 0.00f, 21.10f };
                        islandInfo.upPoint = { -6.0f, 0.0f, 42.0f };
                        islandInfo.rightPoint = { 46.0f, 0.0f, 18.0f };
                        islandInfo.downPoint = { 8.0f, 0.0f, -36.0f };
                    }
					else if (randMedium == 1) {
						islandInfo.name = "MEDIUM2";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -40.60f, 0.00f, 4.70f };
						islandInfo.upPoint = { 2.70f, 0.00f, 39.90f };
						islandInfo.rightPoint = { 45.10f, 29.10f, -6.60f };
						islandInfo.downPoint = { -19.40f, 0.00f, -33.30f };
					}
                    else if (randMedium == 2) {
                        islandInfo.name = "MEDIUM3";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -40.60f, 13.00f, -3.00f };
                        islandInfo.upPoint = { 25.30f, 20.90f, 39.90f };
                        islandInfo.rightPoint = { 45.40f, 0.00f, -12.20f };
                        islandInfo.downPoint = { 6.80f, 3.60f, -34.80f };
                    }
					else if (randMedium == 3) {
						islandInfo.name = "MEDIUM4";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -40.60f, 0.00f, -6.30f };
						islandInfo.upPoint = { 18.80f, 0.00f, 38.70f };
						islandInfo.rightPoint = { 45.40f, 4.30f, -12.20f };
						islandInfo.downPoint = { 3.30f, 0.00f, -35.40f };
					}
					else if (randMedium == 4) {
						islandInfo.name = "MEDIUM5";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -41.00f, 0.00f, 13.40f };
						islandInfo.upPoint = { 18.20f, 4.80f, 38.70f };
						islandInfo.rightPoint = { 45.40f, 4.30f, -15.20f };
						islandInfo.downPoint = { 20.70f, -1.60f, -36.70f };
					}
					else if (randMedium == 5) {
						islandInfo.name = "MEDIUM6";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -41.50f, 0.00f, -6.60f };
						islandInfo.upPoint = { 7.70f, 4.80f, 41.10f };
						islandInfo.rightPoint = { 45.40f, 4.30f, 14.50f };
						islandInfo.downPoint = { -2.80f, 3.20f, -36.70f };
					}
					else if (randMedium == 6) {
						islandInfo.name = "MEDIUM7";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -42.00f, 0.00f, 6.50f };
						islandInfo.upPoint = { -4.80f, 0.00f, 40.50f };
						islandInfo.rightPoint = { 45.40f, 0.00f, 11.10f };
						islandInfo.downPoint = { 1.70f, 4.10f, -36.00f };
					}
					else if (randMedium == 7) {
						islandInfo.name = "MEDIUM8";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -42.00f, 13.80f, 6.50f };
						islandInfo.upPoint = { -4.80f, 12.40f, 40.50f };
						islandInfo.rightPoint = { 45.50f, 0.00f, -6.60f };
						islandInfo.downPoint = { 1.70f, 0.00f, -36.00f };
					}
                    else {
                        islandInfo.name = "MEDIUM9";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -42.00f, 0.00f, 6.50f };
                        islandInfo.upPoint = { 9.40f, 2.90f, 40.50f };
                        islandInfo.rightPoint = { 45.50f, 0.00f, -6.60f };
                        islandInfo.downPoint = { 1.70f, 0.00f, -36.00f };
                    }
                    mediumIslandCount--;
                    break;
                }
                else if (randIsland == 2 && smallIslandCount > 0)
                {
                    spawned = false;
                    int randSmall = rand() % 12;
                    if (randSmall == 0) {
                        islandInfo.name = "SMALL1";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -30.50f, 2.10f, 5.00f };
                        islandInfo.upPoint = { -10.00f, 4.10f, 33.00f };
                        islandInfo.rightPoint = { 34.00f, 0.00f, -10.00f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -26.80f };
                    }
                    else if(randSmall == 1) {
                        islandInfo.name = "SMALL2";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -30.50f, 0.00f, -4.70f };
                        islandInfo.upPoint = { 18.70f, 0.00f, 31.30f };
                        islandInfo.rightPoint = { 33.00f, 0.00f, -8.50f };
                        islandInfo.downPoint = { 0.0f, 0.0f, -25.0f };
                    }
                    else if(randSmall == 2) {
                        islandInfo.name = "SMALL3";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.40f, 0.00f, 8.00f };
                        islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.50f, 0.00f, 1.00f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -26.60f };
                    }
					else if (randSmall == 3) {
						islandInfo.name = "SMALL4";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
						islandInfo.leftPoint = { -29.60f, 0.00f, 7.50f };
						islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
						islandInfo.rightPoint = { 33.00f, 0.00f, -4.40f };
						islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
					}
					else if (randSmall == 4) {
						islandInfo.name = "SMALL5";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.60f, 0.00f, 7.50f };
                        islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.00f, 0.00f, -4.40f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
					}
					else if (randSmall == 5) {
						islandInfo.name = "SMALL6";
						islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.60f, 0.00f, 7.50f };
                        islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.00f, 0.00f, -4.40f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
					}
                    else if (randSmall == 6) {
                        islandInfo.name = "SMALL7";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.60f, 0.00f, 7.50f };
                        islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.00f, 0.00f, -4.40f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
                    }
                    else if (randSmall == 7) {
                        islandInfo.name = "SMALL8";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.60f, 0.00f, 7.50f };
                        islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.00f, 0.00f, -4.40f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
                    }
                    else if (randSmall == 8) {
                        islandInfo.name = "SMALL9";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.60f, 0.00f, 7.50f };
                        islandInfo.upPoint = { 3.00f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.00f, 0.00f, -4.40f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
                    }
                    else if (randSmall == 9) {
                        islandInfo.name = "SMALL10";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -33.0f, 1.5f, -10.5f };
                        islandInfo.upPoint = { 16.20f, 0.00f, 32.60f };
                        islandInfo.rightPoint = { 33.5f, 1.5f, -10.5f };
                        islandInfo.downPoint = { 7.30f, 0.00f, -27.50f };
                    }
                    else if (randSmall == 10) {
                        islandInfo.name = "SMALL11";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -29.30f, 0.00f, -13.80f };
                        islandInfo.upPoint = { 13.70f, 0.00f, 31.10f };
                        islandInfo.rightPoint = { 33.20f, 0.00f, -14.10f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
                    }
                    else {
                        islandInfo.name = "SMALL12";
                        islandInfo.pos = { 0.0f, 0.0f, 0.0f };
                        islandInfo.leftPoint = { -27.70f, 0.00f, -0.40f };
                        islandInfo.upPoint = { 13.70f, 0.00f, 31.10f };
                        islandInfo.rightPoint = { 33.20f, 0.00f, -14.10f };
                        islandInfo.downPoint = { 0.00f, 0.00f, -27.50f };
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
    Vector3 pos1 = island1.pos;
	Vector3 pos2 = island2.pos;

	pos1.x += (island1.rightPoint.x + island1.leftPoint.x)/2;
	pos1.z += (island1.upPoint.z + island1.downPoint.z) / 2;
	pos2.x += (island2.rightPoint.x + island2.leftPoint.x) / 2;
	pos2.z += (island2.upPoint.z + island2.downPoint.z) / 2;

    Vector2 extents1;
	Vector2 extents2;

	extents1.x = (std::abs(island1.rightPoint.x - island1.leftPoint.x) / 2.0f) - 2.0f;
	extents1.y = (std::abs(island1.upPoint.z - island1.downPoint.z) / 2.0f) - 2.0f;
	extents2.x = (std::abs(island2.rightPoint.x - island2.leftPoint.x) / 2.0f) - 2.0f;
	extents2.y = (std::abs(island2.upPoint.z - island2.downPoint.z) / 2.0f) - 2.0f;

    if (std::abs(pos1.x - pos2.x) > (extents1.x + extents2.x)) return false;
    if (std::abs(pos1.z - pos2.z) > (extents1.y + extents2.y)) return false;
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
            int randEnemy = rand() % 2;

            if (randEnemy == 0)
            {
				enemy = PrefabManager::InstantiateMageEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
            }
            else if (randEnemy == 1)
            {
                enemy = PrefabManager::InstantiateFlyingEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
                enemy->GetComponent<StateMachine>()->basePos = pos;
            }

            numberOfHardEnemies--;
        }
        if (randEnemy == 1 && numberOfMediumEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                enemy = PrefabManager::InstantiateShootingEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
                enemy->GetComponent<StateMachine>()->basePos = pos;
            }
            if (randEnemy == 1)
            {
                enemy = PrefabManager::InstantiateTankEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
                enemy->GetComponent<StateMachine>()->basePos = pos;
            }

            numberOfMediumEnemies--;
        }
        if (randEnemy == 2 && numberOfEasyEnemies > 0)
        {
            int randEnemy = rand() % 1;

            if (randEnemy == 0)
            {
                enemy = PrefabManager::InstantiateNormalEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
                enemy->GetComponent<StateMachine>()->basePos = pos;
            }
            if (randEnemy == 1)
            {
                enemy = PrefabManager::InstantiateFastEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
                enemy->GetComponent<StateMachine>()->basePos = pos;
            }
            if (randEnemy == 2)
            {
                enemy = PrefabManager::InstantiateTankEnemy(pOwner, Vector3(0.0f, 0.0f, 0.0f));
                enemy->SetWorldPosition(pos);
                enemy->GetComponent<StateMachine>()->basePos = pos;
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
