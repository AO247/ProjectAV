#include "LevelGenerator.h"
#include "Components.h"

LevelGenerator::LevelGenerator(PrefabManager* prefabManager, Node* root, Node* pPlayer, bool isNew)
    : prefabManager(prefabManager), pSceneRoot(root), pPlayer(pPlayer)
{
    if (prefabManager != nullptr)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        points.push_back(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
        islandGenerated = false;
        enemiesSpawned = false;
        isFinished = false;

        if (!isNew)
        {
            GenerateIslands();
            SpawnEnemies();
        }
    }
}

void LevelGenerator::Update()
{

    if (!islandGenerated)
    {
        GenerateIslandsNew();
    }

    if(islandGenerated)
    {
		isFinished = true;
    }
}
void LevelGenerator::GenerateIslandsNew()
{
    int randIsland = rand() % 3;
    Node* islandPrefab = nullptr;
    if (spawned) {
        spawned = false;
        if (islands.size() > 0)
        {
            while (true) {

                if (randIsland == 0 && bigIslandCount > 0)
                {
                    int randLarge = rand() % 2;
                    if (randLarge == 0) {
                        //islandPrefab = PrefabManager::InstantiateIslandBig1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    else {
                       // islandPrefab = prefabManager->InstantiateIslandBig2(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    bigIslandCount--;
                    break;
                }
                else if (randIsland == 1 && mediumIslandCount > 0)
                {
                    int randMedium = rand() % 2;
                    if (randMedium == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandMedium1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandMedium1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    mediumIslandCount--;
                    break;
                }
                else if (randIsland == 2 && smallIslandCount > 0)
                {
                    int randSmall = rand() % 2;
                    if (randSmall == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandSmall1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.5f);
                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandSmall2(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.5f);
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
            islandPrefab = PrefabManager::InstantiateFirstIsland(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }

    if (islandPrefab != nullptr && islandPrefab->GetComponent<Island>())
    {
        Island* island = islandPrefab->GetComponent<Island>();

        int randPoint = rand() % points.size();

        Vector4 point = points[randPoint];
        Vector3 pointPos = Vector3(point.x, point.y, point.z);
        islandPrefab->SetWorldPosition(pointPos);

        if (pointPos == Vector3(0.0f, 0.0f, 0.0f))
        {

            ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
            if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                points.erase(points.begin() + randPoint);
            }
            points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
            points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
            points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
            islands.push_back(islandPrefab);
			spawned = true;
        }
        else {
            island->Rotate();
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
                    points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
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
                    points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
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
                    points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
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
                    points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
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
    if (bigIslandCount <= 0 && mediumIslandCount <= 0 && smallIslandCount <= 0)
    {
        OutputDebugStringA("\nAll islands generated\n");

        for (int i = 0; i < islands.size(); i++)
        {
            float randY = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 10.0f);
            islands[i]->SetLocalPosition(DirectX::XMFLOAT3(islands[i]->GetLocalPosition().x, islands[i]->GetLocalPosition().y + randY, islands[i]->GetLocalPosition().z));
        }
		islandGenerated = true;
    }
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
            //time = 0.0f;
            break;
        }
        int randIsland = rand() % 3;
        Node* islandPrefab = nullptr;
        if (islands.size() > 0)
        {
            while (true) {

                if (randIsland == 0 && bigIslandCount > 0)
                {
                    int randLarge = rand() % 2;
                    if (randLarge == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandBig1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandBig2(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    bigIslandCount--;
                    break;
                }
                else if (randIsland == 1 && mediumIslandCount > 0)
                {
                    int randMedium = rand() % 2;
                    if (randMedium == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandMedium1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandMedium1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
                    }
                    mediumIslandCount--;
                    break;
                }
                else if (randIsland == 2 && smallIslandCount > 0)
                {
                    int randSmall = rand() % 2;
                    if (randSmall == 0) {
                        //islandPrefab = prefabManager->InstantiateIslandSmall1(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.5f);
                    }
                    else {
                        //islandPrefab = prefabManager->InstantiateIslandSmall2(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.5f);
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
			islandPrefab = prefabManager->InstantiateFirstIsland(pSceneRoot, 0.0f, 0.0f, 0.0f, 1.0f);
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
                    //points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                    points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                    time = 0.0f;
					islands.push_back(islandPrefab);
                    //dodaæ warunek sprawdzenia czy nie wchodzi w kolizje
                    break;
                }
            }
            else {
                island->Rotate();
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
                        /*Vec3 jPos = Vec3(islandPrefab->GetWorldPosition().x, islandPrefab->GetWorldPosition().y, islandPrefab->GetWorldPosition().z);
                        Vec3 jExtents = Vec3(10.0f, 2.0f, 10.f);
                        Quat quat = Quat(Vec4Arg(islandPrefab->GetLocalRotationQuaternion().x, islandPrefab->GetLocalRotationQuaternion().y, islandPrefab->GetLocalRotationQuaternion().z, islandPrefab->GetLocalRotationQuaternion().w));
                        BodyIDVector bodies = PhysicsCommon::OverlapBox(jPos, jExtents, quat);*/
                        bool flag = true;
                        for (int i = 0; i < islands.size(); i++)
                        {
							if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
							{
								OutputDebugStringA(("\nCollides with: \n" + islands[i]->GetName() + " current body: " + islandPrefab->GetName()).c_str());
                                flag = false;
                            }

                        }
                        if (!flag)
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
                        }
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt gorny z dolem\n");
                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        //points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                        points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                        points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        time = 0.0f;
                        islands.push_back(islandPrefab);
                        break;
                    }
                    else if (point.w == 1)
                    {
                        ChangePosition(islandPrefab, pointPos, island->rightPoint->GetWorldPosition());
                        bool flag = true;
                        for (int i = 0; i < islands.size(); i++)
                        {
                            if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                            {
                                OutputDebugStringA(("\nCollides with: \n" + islands[i]->GetName() + " current body: " + islandPrefab->GetName()).c_str());
                                flag = false;
                            }

                        }
                        if (!flag)
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
                        }
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt prawy z lewym\n");

                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                        //points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                        points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                        time = 0.0f;
                        islands.push_back(islandPrefab);
                        break;
                    }
                    else if (point.w == 2)
                    {
                        ChangePosition(islandPrefab, pointPos, island->downPoint->GetWorldPosition());
                        bool flag = true;
                        for (int i = 0; i < islands.size(); i++)
                        {
                            if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                            {
                                OutputDebugStringA(("\nCollides with: \n" + islands[i]->GetName() + " current body: " + islandPrefab->GetName()).c_str());
                                flag = false;
                            }

                        }
                        if (!flag)
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
                        }
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt dolny z gornym\n");

                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->leftPoint->GetWorldPosition().x, island->leftPoint->GetWorldPosition().y, island->leftPoint->GetWorldPosition().z, 1.0f));
                        //points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                        points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        time = 0.0f;
                        islands.push_back(islandPrefab);
                        break;
                    }
                    else if (point.w == 3)
                    {
                        ChangePosition(islandPrefab, pointPos, island->leftPoint->GetWorldPosition());
                        bool flag = true;
                        for (int i = 0; i < islands.size(); i++)
                        {
                            if (Collide(islandPrefab, islands[i]) && islands[i] != islandPrefab)
                            {
                                OutputDebugStringA(("\nCollides with: \n" + islands[i]->GetName() + " current body: " + islandPrefab->GetName()).c_str());
                                flag = false;
                            }

                        }
                        if (!flag)
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
                        }
                        OutputDebugStringA(("\nZnaleziono miejsce dla wyspy: " + islandPrefab->GetName()).c_str());
                        OutputDebugString("\nPolaczono punkt lewy z prawym\n");

                        if (randPoint >= 0 && randPoint < static_cast<int>(points.size())) {
                            points.erase(points.begin() + randPoint);
                        }
                        points.push_back(Vector4(island->downPoint->GetWorldPosition().x, island->downPoint->GetWorldPosition().y, island->downPoint->GetWorldPosition().z, 0.0f));
                        points.push_back(Vector4(island->upPoint->GetWorldPosition().x, island->upPoint->GetWorldPosition().y, island->upPoint->GetWorldPosition().z, 2.0f));
                        //points.push_back(Vector4(island->rightPoint->GetWorldPosition().x, island->rightPoint->GetWorldPosition().y, island->rightPoint->GetWorldPosition().z, 3.0f));
                        time = 0.0f;
                        islands.push_back(islandPrefab);
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

			for (int i = 0; i < islands.size(); i++)
			{
                float randY = -5.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / 10.0f);
                islands[i]->SetLocalPosition(DirectX::XMFLOAT3(islands[i]->GetLocalPosition().x, islands[i]->GetLocalPosition().y + randY, islands[i]->GetLocalPosition().z));
			}
            break;
        }
    }
}

void LevelGenerator::ChangePosition(Node* island, Vector3 pointPos, Vector3 startPos)
{
    Vector3 pos = island->GetWorldPosition();

    Vector3 dir = pointPos - startPos;
    float length = dir.Length();
    dir.Normalize();
    island->SetLocalPosition(island->GetLocalPosition() + (dir * length));
    pos = island->GetWorldPosition();
}

bool LevelGenerator::Collide(Node* island1, Node* island2) 
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


void LevelGenerator::SpawnEnemies() 
{
    OutputDebugStringA("\n\n\nRozpoczynamy spawn enemies\n\n\n");

    int islandNumber = 0;
    float time = 0.0f;
	Vector3 startPos = Vector3(0.0f, 0.0f, 15.0f);
    while (numberOfEasyEnemies > 0 || numberOfMediumEnemies > 0 || numberOfHardEnemies > 0) {
		int randEnemy = rand() % 1; // 0 - hard, 1 - medium, 2 - easy
		randEnemy = 0; // Force hard enemy for now
        Node* enemy = nullptr;
        if (randEnemy == 0 && numberOfHardEnemies > 0)
        {
            Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
            int pIslandNumber = islandNumber;
			std::vector<Node*> spawnPoints;
			spawnPoints = islands[islandNumber]->GetComponent<Island>()->spawnPoints;
            while (true)
            {
                if (time > 20.0f)
                {
                    OutputDebugStringA("\nBrak czasu\n");
                    time = 0.0f;
                    break;
                }
                if (spawnPoints.size() > 0)
                {
					int randSpot = rand() % spawnPoints.size();
                    pos = spawnPoints[randSpot]->GetWorldPosition();
                    // Replace this line:

                    if (randSpot >= 0 && randSpot < static_cast<int>(spawnPoints.size())) {
                        spawnPoints.erase(spawnPoints.begin() + randSpot);
                        islands[islandNumber]->GetComponent<Island>()->spawnPoints = spawnPoints;
                    }
                    if ((pos - startPos).Length() > 10.0f)
                    {
                        OutputDebugStringA("\nZa blisko\n");
                        islandNumber++;
                        time = 0.0f;
                        break;
                    }
                }
                else
                {
                    islandNumber++;
                    if (islandNumber == pIslandNumber)
                    {
                        return;
                    }
                    spawnPoints = islands[islandNumber]->GetComponent<Island>()->spawnPoints;
                }

            }
			int randEnemy = rand() % 1;
			randEnemy = 0; // Force hard enemy for now

            if(randEnemy == 0)
            {
                OutputDebugStringA("\nDodajemy\n");
                //enemy = prefabManager->InstantiateShootingEnemy(pSceneRoot, pos.x, pos.y, pos.z, 1.6f, pPlayer);
            }
            numberOfHardEnemies--;
        }
        if (islandNumber == islands.size())
        {
            islandNumber = 0;
        }

        if (numberOfEasyEnemies <= 0 && numberOfMediumEnemies <= 0 && numberOfHardEnemies <= 0)
        {
            OutputDebugStringA("\nAll enemies spawned\n");
            break;
        }

    }
}
