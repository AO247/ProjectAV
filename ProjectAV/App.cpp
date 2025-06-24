#include "App.h"     
#include "State.h"
#include "CMath.h"
#include "Surface.h"
#include "imgui/imgui.h"
#include <algorithm>
#include "CUtil.h"
#include "ColliderSphere.h"
#include "TexturePreprocessor.h"
#include "SolidCapsule.h"
#include "SoundDevice.h"
#include "SoundEffectsPlayer.h"
#include "StaticSoundPlayer.h"
#include "DebugLine.h"
#include "Testing.h"
#include "PrefabManager.h"
#include "LevelGenerator.h"
#include <Jolt/Jolt.h>
#include <Jolt/ConfigurationString.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include "imgui/imgui_impl_dx11.h"

namespace dx = DirectX;

App::App(const std::string& commandLine)
    :
    commandLine(commandLine),
    wnd(1920, 1080, "Project AV"), 
    pointLight(wnd.Gfx(), 2u), 
    dirLight(wnd.Gfx(), 0u),
    pSceneRoot(std::make_unique<Node>("Root"))
{

    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 2000.0f));


    std::srand(static_cast<unsigned>(std::time(nullptr)));
    pointLight.LinkTechniques(rg);
     pointLight.cbData.enabled = FALSE; 

    RegisterDefaultAllocator();
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)
        Factory::sInstance = new JPH::Factory();
    RegisterTypes();
    temp_allocator = new TempAllocatorImpl(10 * 2024 * 1024);
    job_system = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
    const uint cMaxBodies = 2048;
    const uint cNumBodyMutexes = 0;
    const uint cMaxBodyPairs = 4096;
    const uint cMaxContactConstraints = 2048;
    BPLayerInterfaceImpl* broad_phase_layer_interface = new BPLayerInterfaceImpl();
    ObjectVsBroadPhaseLayerFilterImpl* object_vs_broadphase_layer_filter = new ObjectVsBroadPhaseLayerFilterImpl();
    ObjectLayerPairFilterImpl* object_vs_object_layer_filter = new ObjectLayerPairFilterImpl();
    physicsSystem = new PhysicsSystem();
    physicsSystem->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *broad_phase_layer_interface, *object_vs_broadphase_layer_filter, *object_vs_object_layer_filter);
    contactListener = new MyContactListener();
    physicsSystem->SetContactListener(contactListener);
    PhysicsCommon::physicsSystem = physicsSystem;
    PrefabManager::wind = &wnd;
    PrefabManager::rg = &rg;
    physicsSystem->SetGravity(Vec3(0.0f, -80.0f, 0.0f));
    //PrefabManager::PreloadAllModels();

    soundDevice = LISTENER->Get();
    ALint attentuation = AL_INVERSE_DISTANCE_CLAMPED;
	soundDevice->SetAttenuation(attentuation);
	StaticSoundPlayer::Get().Init(64);
    myMusic = std::make_unique<MusicBuffer>("Music\\windererfull.mp3");
    myMusic->setGain(1.0f);

	auto base = std::make_unique<Node>("Base");
	auto playerThings = std::make_unique<Node>("Player Things");
    auto pCameraNodeOwner = std::make_unique<Node>("Camera", nullptr, "CAMERA");
    pCamera = pCameraNodeOwner.get();
    auto pFreeViewCameraOwner = std::make_unique<Node>("FreeViewCamera");
    pFreeViewCamera = pFreeViewCameraOwner.get();
    auto pPlayerOwner = std::make_unique<Node>("Player", nullptr, "PLAYER");
    pPlayer = pPlayerOwner.get();
	auto abilities = std::make_unique<Node>("Abilities");
    auto pAbility1Owner = std::make_unique<Node>("Ability1", nullptr, "TRIGGER");
    Node* pAbility1 = pAbility1Owner.get();
    auto pAbility2Owner = std::make_unique<Node>("Ability2", nullptr, "TRIGGER");
    Node* pAbility2 = pAbility2Owner.get();
    auto pAbility3Owner = std::make_unique<Node>("Ability3", nullptr, "TRIGGER");
    Node* pAbility3 = pAbility3Owner.get();
    auto pAbility4Owner = std::make_unique<Node>("Ability4", nullptr, "TRIGGER");
    Node* pAbility4 = pAbility4Owner.get();
	auto pAbility5Owner = std::make_unique<Node>("Ability5", nullptr, "TRIGGER");
    Node* pAbility5 = pAbility5Owner.get();
    auto pAbility6Owner = std::make_unique<Node>("Ability6", nullptr, "TRIGGER");
    Node* pAbility6 = pAbility6Owner.get();
    auto pPrefabsOwner = std::make_unique<Node>("Temporary", nullptr, "PREFABS");
    Node* pPrefabs = pPrefabsOwner.get();
    auto pLeftHandNormalOwner = std::make_unique<Node>("L Normal", nullptr, "HANDS");
    pLeftHandNormal = pLeftHandNormalOwner.get();
    auto pLeftHandAbilityOwner = std::make_unique<Node>("L Ability", nullptr, "HANDS");
    pLeftHandAbility = pLeftHandAbilityOwner.get();
    auto pRightHandNormalOwner = std::make_unique<Node>("R Normal", nullptr, "HANDS");
    pRightHandNormal = pRightHandNormalOwner.get();
    auto pRightHandAbilityOwner = std::make_unique<Node>("R Ability", nullptr, "HANDS");
    pRightHandAbility = pRightHandAbilityOwner.get();

	Node* pPlayerThings = playerThings.get();
	Node* pAbilities = abilities.get();
	Node* pBase = base.get();
    pSceneRoot->AddChild(std::move(pPrefabsOwner));
	pSceneRoot->AddChild(std::move(base));
    pSceneRoot->AddChild(std::move(playerThings));
    pPlayerThings->AddChild(std::move(pCameraNodeOwner));
    pPlayerThings->AddChild(std::move(pFreeViewCameraOwner));
    pPlayerThings->AddChild(std::move(pPlayerOwner));
	pSceneRoot->AddChild(std::move(abilities));
    pAbilities->AddChild(std::move(pAbility1Owner));
    pAbilities->AddChild(std::move(pAbility2Owner));
    pAbilities->AddChild(std::move(pAbility3Owner));
    pAbilities->AddChild(std::move(pAbility4Owner));
    pAbilities->AddChild(std::move(pAbility5Owner));
    pAbilities->AddChild(std::move(pAbility6Owner));
    pCamera->AddChild(std::move(pLeftHandNormalOwner));
    pCamera->AddChild(std::move(pLeftHandAbilityOwner));
    pCamera->AddChild(std::move(pRightHandNormalOwner));
    pCamera->AddChild(std::move(pRightHandAbilityOwner));

    //PrefabManager::InstantiateStone1(pSceneRoot.get(), Vector3(0.0f, 100.0f, 0.0f), 1.0f);

    PrefabManager::root = pPrefabs;
    PrefabManager::player = pPlayer;

    BodyCreationSettings bodySettings(new JPH::CapsuleShape(1.0f, 1.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::PLAYER);
    bodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    bodySettings.mMassPropertiesOverride.mMass = 1.0f;
    bodySettings.mFriction = 0.0f;
    bodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
    bodySettings.mMotionQuality = EMotionQuality::LinearCast;
    pPlayer->AddComponent(
        std::make_unique<Rigidbody>(pPlayer, bodySettings)
    );
    Rigidbody* pRigidbody = pPlayer->GetComponent<Rigidbody>();

    pPlayer->AddComponent(
        std::make_unique<PlayerController>(pPlayer, wnd)
    );

    BodyCreationSettings a1BodySettings(new JPH::CapsuleShape(6.0f, 5.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility1->AddComponent(
        std::make_unique<Trigger>(pAbility1, a1BodySettings, false)
    );
    pAbility1->AddComponent(
        std::make_unique<Ability1>(pAbility1, wnd, pCamera)
    );
    pAbility1->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f));
    pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility1;


    BodyCreationSettings a2odySettings(new JPH::SphereShape(5.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility2->AddComponent(
        std::make_unique<Trigger>(pAbility2, a2odySettings, false)
    );
    pAbility2->AddComponent(
        std::make_unique<Ability2>(pAbility2, wnd, pCamera)
    );

    pPlayer->GetComponent<PlayerController>()->abilitySlot2 = pAbility2;


    BodyCreationSettings a3odySettings(new JPH::SphereShape(40.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility3->AddComponent(
        std::make_unique<Trigger>(pAbility3, a3odySettings, false)
    );
    pAbility3->AddComponent(
        std::make_unique<Ability3>(pAbility3, wnd, pCamera)
    );
    pPlayer->GetComponent<PlayerController>()->abilitySlot3 = pAbility3;


    BodyCreationSettings a4odySettings(new JPH::SphereShape(2.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility4->AddComponent(
        std::make_unique<Trigger>(pAbility4, a4odySettings, false)
    );

    pAbility4->AddComponent(
        std::make_unique<Ability4>(pAbility4, wnd, pCamera)
    );
    /*pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility4;
    pAbility4->GetComponent<Ability4>()->baseAbility = pAbility1->GetComponent<Ability1>();*/


    pAbility5->AddComponent(
        std::make_unique<Ability5>(pAbility5, wnd, pCamera)
    );
	//pPlayer->GetComponent<PlayerController>()->abilitySlot2 = pAbility5;

    pAbility6->AddComponent(
        std::make_unique<Ability6>(pAbility6, wnd, pCamera)
    );
    /*pAbility6->GetComponent<Ability6>()->baseAbility = pAbility1->GetComponent<Ability1>();
    pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility6;*/

    pFreeViewCamera->AddComponent(
        std::make_unique<Camera>(pFreeViewCamera, wnd)
    );
    pCamera->AddComponent(
        std::make_unique<Camera>(pCamera, wnd)
    );
    pCamera->GetComponent<Camera>()->active = true;


    pPlayer->AddComponent(
        std::make_unique<Health>(pPlayer, 3.0f)
    );

    pPlayer->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pPlayer)
    );
	StaticSoundPlayer::Get().SetPlayerNode(pPlayer);
    SoundEffectsPlayer* pSoundEffectsPlayer = pPlayer->GetComponent<SoundEffectsPlayer>();
    pSoundEffectsPlayer->AddSound("Models\\turn.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\push1.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\push2.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\toss1.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\toss2.ogg");

    pFreeViewCamera->SetLocalPosition({ 4.0f, 11.0f, -28.0f });
    pPlayer->SetLocalPosition({ 0.0f, 80.0f, -24.0f });

    pSceneRoot->AddComponent(
        std::make_unique<Global>(pSceneRoot.get(), wnd, pPlayer, pBase)
    );

    pLeftHandNormal->AddComponent(
        std::make_unique<ModelComponent>(pLeftHandNormal, wnd.Gfx(), "Models\\hands\\left.obj")
    );
    pLeftHandNormal->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pLeftHandNormal->SetLocalScale({ 0.1f, 0.1f, 0.1f });
    pLeftHandNormal->SetLocalPosition({ 0.0f, -2.7f, 3.0f });

    pLeftHandAbility->AddComponent(
        std::make_unique<ModelComponent>(pLeftHandAbility, wnd.Gfx(), "Models\\hands\\push.obj")
    );
    pLeftHandAbility->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pLeftHandAbility->SetLocalScale({ 0.1f, 0.1f, 0.1f });
    pLeftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });

    pRightHandNormal->AddComponent(
        std::make_unique<ModelComponent>(pRightHandNormal, wnd.Gfx(), "Models\\hands\\right.obj")
    );
    pRightHandNormal->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pRightHandNormal->SetLocalScale({ 0.1f, 0.1f, 0.1f });
    pRightHandNormal->SetLocalPosition({ 0.0f, -2.7f, 3.0f });

    pRightHandAbility->AddComponent(
        std::make_unique<ModelComponent>(pRightHandAbility, wnd.Gfx(), "Models\\hands\\toss.obj")
    );
    pRightHandAbility->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pRightHandAbility->SetLocalScale({ 0.1f, 0.1f, 0.1f });
    pRightHandAbility->SetLocalPosition({ 0.0f, -2.7f, -3000.0f });

    pAbility1->GetComponent<Ability1>()->leftHandNormal = pLeftHandNormal;
    pAbility1->GetComponent<Ability1>()->leftHandAbility = pLeftHandAbility;

    pAbility2->GetComponent<Ability2>()->rightHandNormal = pRightHandNormal;
    pAbility2->GetComponent<Ability2>()->rightHandAbility = pRightHandAbility;

    pAbility4->GetComponent<Ability4>()->leftHandNormal = pLeftHandNormal;
    pAbility4->GetComponent<Ability4>()->leftHandAbility = pLeftHandAbility;

	pAbility5->GetComponent<Ability5>()->rightHandNormal = pRightHandNormal;
	pAbility5->GetComponent<Ability5>()->rightHandAbility = pRightHandAbility;

    pAbility6->GetComponent<Ability6>()->leftHandNormal = pLeftHandNormal;
    pAbility6->GetComponent<Ability6>()->leftHandAbility = pLeftHandAbility;

    pSceneRoot->AddComponent(
        std::make_unique<UpgradeHandler>(pSceneRoot.get(), wnd)
    );
    pUpgradeHandler = pSceneRoot->GetComponent<UpgradeHandler>();
    pUpgradeHandler->ability1Node = pAbility1;
    pUpgradeHandler->ability2Node = pAbility2;
    pUpgradeHandler->ability3Node = pAbility3;
    pUpgradeHandler->ability4Node = pAbility4;
	pUpgradeHandler->ability5Node = pAbility5;
	pUpgradeHandler->ability6Node = pAbility6;
    pUpgradeHandler->playerController = pPlayer->GetComponent<PlayerController>();
    pUpgradeHandler->SetBasicValues();
    pSceneRoot->GetComponent<Global>()->upgradeHandler = pUpgradeHandler;

	//PrefabManager::InstantiateIslandBig10(pSceneRoot.get(), Vector3(0.0f, 0.0f, 0.0f), 1.0f);
	//tutorialNode = PrefabManager::InstantiateTutorialIslands(pSceneRoot.get(), Vector3(0.0f, 0.0f, 0.0f), 1.0f);
    //pSceneRoot->GetComponent<Global>()->tut = tutorialNode->GetComponent<Tutorial>();


    const int screenWidth = 1920;
    const int screenHeight = 1080;
    const int plusSpriteWidth = 32;
    const int plusSpriteHeight = 32;
    const int plusSpriteX = (screenWidth / 2) - (plusSpriteWidth / 2);
    const int plusSpriteY = (screenHeight / 2) - (plusSpriteHeight / 2);

    targetSprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        plusSpriteX,                
        plusSpriteY,                
        plusSpriteWidth,            
        plusSpriteHeight,           
        L"Images\\plus.png"
    );

    heart1Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        (screenWidth / 2) - 35 - 80,             
        950,        
        70,         
        70,         
        L"Images\\heart.png"
    );

    heart2Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        (screenWidth / 2) - 35,              
        950,        
        70,         
        70,         
        L"Images\\heart.png"
    );

    heart3Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),    
        wnd.Gfx().GetContext(),
        (screenWidth / 2) - 35 + 80,               
        950,         
        70,          
        70,          
        L"Images\\heart.png"
    );
    loadingScreen1 = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        0,
        0,
        1920,
        1080,
        L"Images\\Loading_Screen1.png"
    );
    loadingScreen2 = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        0,
        0,
        1920,
        1080,
        L"Images\\Loading_Screen2.png"
    );
    loadingScreen3 = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        0,
        0,
        1920,
        1080,
        L"Images\\Loading_Screen3.png"
    );

    wnd.DisableCursor();
    wnd.mouse.EnableRawInput();
    cursorEnabled = false;

}

App::~App()
{
	StaticSoundPlayer::Get().Shutdown();
    std::ofstream outFile("scene_transforms.txt");
    if (outFile.is_open())
    {
        if (pSceneRoot)
        {
            // Rozpocznij rekurencyjny zapis od korzenia sceny
            SaveNodeTransformsRecursive(*pSceneRoot, outFile);
        }
        outFile.close();
    }
}

int App::Go()
{
    const float FIXED_TIME_STEP = 1.0f / 60.0f;
    float lag = 0.0f;

    while (true)
    {
        if (const auto ecode = Window::ProcessMessages())
        {
            return *ecode;
        }

        const float dt = timer.Mark();
        lag += dt;

        constexpr float MAX_LAG = 0.5f;
        if (lag > MAX_LAG)
            lag = MAX_LAG;

        while (lag >= FIXED_TIME_STEP)
        {
            physicsSystem->Update(FIXED_TIME_STEP, 1, temp_allocator, job_system);
            lag -= FIXED_TIME_STEP;
        }

        const float alpha = lag / FIXED_TIME_STEP;

        HandleInput(dt);
        DoFrame(dt);

    }
}

void App::HandleInput(float dt)
{


    while (const auto e = wnd.kbd.PollKeyEvent())
    {
        if (!e->IsPress()) continue;

        switch (e->GetCode())
        {
        case 'P':
        {
            PrefabManager::InstantiateIslandBig1(pSceneRoot.get(), pFreeViewCamera->GetWorldPosition(), 1.0f);
			PrefabManager::InstantiateNormalEnemy(pSceneRoot.get(), pFreeViewCamera->GetWorldPosition(), 1.0f);
            break;
        }

		case 'M':
			if (myMusic->isPlaying())
			{
				myMusic->Stop();
			}
			else
			{
				myMusic->Play();
			}
			break;
        case 'C':
            if (cursorEnabled) {
                wnd.DisableCursor();
                wnd.mouse.EnableRawInput();
            }
            else {
                wnd.EnableCursor();
                wnd.mouse.DisableRawInput();
            }
            cursorEnabled = !cursorEnabled;
            break;
        case 'H': 
            showControlWindow = !showControlWindow;
            break;
        case 'X':
            tutorialNode = PrefabManager::InstantiateTutorialIslands(pSceneRoot.get(), Vector3(0.0f, 0.0f, 0.0f), 1.0f);
            break;
        /*case 'Q':
			tutorialNode->GetComponent<Tutorial>()->qPressed= true;
			break;*/
        /*case 'B':
            if (pPlayer->GetComponent<PlayerController>()->abilitySlot1 == pAbility1)
            {
                pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility4;
            }
            else
            {
                pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility1;
            }
            break;*/
        case VK_ESCAPE:
            PostQuitMessage(0);
            return;
        case VK_F1:
            showDemoWindow = !showDemoWindow;
            break;
        case 'V':
        {
            freeViewCamera = !freeViewCamera;
            if (freeViewCamera) {
                pCamera->GetComponent<Camera>()->active = false;
                pFreeViewCamera->GetComponent<Camera>()->active = true;
            }
            else {
                pCamera->GetComponent<Camera>()->active = true;
                pFreeViewCamera->GetComponent<Camera>()->active = false;
            }
            break;
        }
        }
    }

    if (freeViewCamera) {
        if (wnd.kbd.IsKeyPressed('I')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.0f, 0.4f });
        }
        if (wnd.kbd.IsKeyPressed('K')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.0f, -0.4f });
        }
        if (wnd.kbd.IsKeyPressed('J')) {
            pFreeViewCamera->TranslateLocal({ -0.4f, 0.0f, 0.0f });
        }
        if (wnd.kbd.IsKeyPressed('L')) {
            pFreeViewCamera->TranslateLocal({ 0.4f, 0.0f, 0.0f });
        }
        if (wnd.kbd.IsKeyPressed('U')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, -0.4f, 0.0f });
        }
        if (wnd.kbd.IsKeyPressed('O')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.4f, 0.0f });
        }
    }
}

void App::DoFrame(float dt)
{
    //fizyka
    auto* contact = dynamic_cast<MyContactListener*>(physicsSystem->GetContactListener());
    contact->ExecuteTriggerActivationQueue();
    contact->ExecuteCollisionActivationQueue();
    CleanupDestroyedNodes(pSceneRoot.get());    //data removed
	pSceneRoot->Update(dt);
    RemoveRigidbody(pSceneRoot.get());//rigidbody remove if destruction

    wnd.Gfx().BeginFrame(0.5f, 0.5f, 1.0f);

    dx::XMMATRIX viewMatrix = pCamera->GetComponent<Camera>()->GetViewMatrix();
    if (freeViewCamera)
    {
        viewMatrix = pFreeViewCamera->GetComponent<Camera>()->GetViewMatrix();
    }
    wnd.Gfx().SetCamera(viewMatrix);

    dirLight.Bind(wnd.Gfx(), viewMatrix);

    FrustumCalculating(); 



    if (myMusic->isPlaying())
    {
        myMusic->UpdateBufferStream();
    }
    if (pPlayer != nullptr) {
        soundDevice->SetLocation(
            pPlayer->GetWorldPosition().x,
            pPlayer->GetWorldPosition().y,
            pPlayer->GetWorldPosition().z
        );

        soundDevice->SetOrientation(
            pCamera->Back().x,
            pCamera->Back().y,
            pCamera->Back().z,
            pCamera->Up().x,
            pCamera->Up().y,
            pCamera->Up().z
        );
    }

    rg.Execute(wnd.Gfx());

    if (showControlWindow) {
        ShowControlWindows();
    }

    

    if (targetSprite ) { 
        targetSprite->Draw(wnd.Gfx().GetContext());
    }

    if (pPlayer->GetComponent<Health>()->currentHealth == 3.0f) {
        heart1Sprite->Draw(wnd.Gfx().GetContext());
        heart2Sprite->Draw(wnd.Gfx().GetContext());
        heart3Sprite->Draw(wnd.Gfx().GetContext());
    }
    if (pPlayer->GetComponent<Health>()->currentHealth == 2.0f) {
        heart1Sprite->Draw(wnd.Gfx().GetContext());
        heart2Sprite->Draw(wnd.Gfx().GetContext());
    }
    if (pPlayer->GetComponent<Health>()->currentHealth == 1.0f) {
        heart1Sprite->Draw(wnd.Gfx().GetContext());
    }

    pUpgradeHandler->DrawUpgradeMenu();

    
	//tutorialNode->GetComponent<Tutorial>()->DrawNote();

    if(pSceneRoot->GetComponent<Global>()->drawLoadingScreen || bonusTime > 0.0f)
    {
        if (!pSceneRoot->GetComponent<Global>()->drawLoadingScreen)
        {
            bonusTime -= dt;
        }
        else {
            bonusTime = 5.0f;
        }
        countLoding ++;

        if (countLoding > 2.0f)
        {
            if (countLoding > 2.9f)
            {
                countLoding = 0.0f;
            }
            loadingScreen3->Draw(wnd.Gfx().GetContext());
        }
        else if (countLoding > 1.0f)
        {
            loadingScreen2->Draw(wnd.Gfx().GetContext());
        }
        else
        {
            loadingScreen1->Draw(wnd.Gfx().GetContext());
        }
    }

    wnd.Gfx().EndFrame();
    rg.Reset();
}

void App::RemoveRigidbody(Node* currentNode)
{
    if (!currentNode) return;

    for (int i = 0; i < currentNode->GetChildren().size(); ++i) {
        RemoveRigidbody(currentNode->GetChildren()[i].get());
    }

    if (currentNode->markedForDestruction)
    {
        if (currentNode->GetComponent<Rigidbody>() != nullptr) 
        {
            PhysicsCommon::physicsSystem->GetBodyInterface().DeactivateBody(currentNode->GetComponent<Rigidbody>()->GetBodyID());
            PhysicsCommon::physicsSystem->GetBodyInterface().RemoveBody(currentNode->GetComponent<Rigidbody>()->GetBodyID());
            PhysicsCommon::physicsSystem->GetBodyInterface().DestroyBody(currentNode->GetComponent<Rigidbody>()->GetBodyID());

        }
    }
}

void App::FrustumCalculating() {
    dx::XMMATRIX camWorldTransform = pCamera->GetWorldTransform();
    dx::XMVECTOR camWorldPos = camWorldTransform.r[3];
    dx::XMVECTOR camWorldForward = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), camWorldTransform)); 
    dx::XMVECTOR camWorldUp = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), camWorldTransform));    
    dx::XMVECTOR camWorldRight = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), camWorldTransform)); 

    constexpr float fovAngleY = DirectX::XMConvertToRadians(70.0f);
    const float aspectRatio = 16.0f / 9.0f; 
    const float nearDist = 0.5f;        
    const float farDist = 800.0f;      

    float halfHeightNear = nearDist * tanf(fovAngleY * 0.5f);
    float halfWidthNear = halfHeightNear * aspectRatio;
    float halfHeightFar = farDist * tanf(fovAngleY * 0.5f);
    float halfWidthFar = halfHeightFar * aspectRatio;

    dx::XMVECTOR nearCenter = dx::XMVectorAdd(camWorldPos, dx::XMVectorScale(camWorldForward, nearDist));
    dx::XMVECTOR farCenter = dx::XMVectorAdd(camWorldPos, dx::XMVectorScale(camWorldForward, farDist));

    dx::XMVECTOR nearTopLeft = dx::XMVectorAdd(nearCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearTopRight = dx::XMVectorAdd(nearCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearBottomLeft = dx::XMVectorAdd(nearCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, -halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearBottomRight = dx::XMVectorAdd(nearCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, -halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR farTopLeft = dx::XMVectorAdd(farCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farTopRight = dx::XMVectorAdd(farCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farBottomLeft = dx::XMVectorAdd(farCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, -halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farBottomRight = dx::XMVectorAdd(farCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, -halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));

    cameraFrustum.Near = 0.5f;

    cameraFrustum.Far = 800.0f;

    float tanHalfFovY = tanf(fovAngleY * 0.5f);
    cameraFrustum.TopSlope = tanHalfFovY;
    cameraFrustum.BottomSlope = -tanHalfFovY;

    cameraFrustum.RightSlope = tanHalfFovY * aspectRatio;
    cameraFrustum.LeftSlope = -tanHalfFovY * aspectRatio;

    DirectX::XMStoreFloat3(&cameraFrustum.Origin, camWorldPos);
    dx::XMMATRIX worldOrientationMatrix;
    worldOrientationMatrix.r[0] = camWorldRight;   
    worldOrientationMatrix.r[1] = camWorldUp;      
    worldOrientationMatrix.r[2] = camWorldForward; 
    worldOrientationMatrix.r[3] = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); 

    DirectX::XMStoreFloat4(&cameraFrustum.Orientation, dx::XMQuaternionRotationMatrix(worldOrientationMatrix));


    DrawNodeRecursive(wnd.Gfx(), pSceneRoot.get());
}

void App::DrawNodeRecursive(Graphics& gfx, Node* node)
{

    bool shouldDraw = true; 
    ModelComponent* modelComp = node->GetComponent<ModelComponent>();

    if (modelComp != nullptr)
    {
        DirectX::BoundingSphere sphere;
        DirectX::BoundingBox box;
        DirectX::ContainmentType containment = DirectX::DISJOINT;

        sphere.Center = node->GetWorldPosition();
        sphere.Radius = node->radius;
        containment = cameraFrustum.Contains(sphere);
        

        if (containment == DirectX::DISJOINT) 
        {
            shouldDraw = false; 
        }

    }

    if (shouldDraw)
    {
        node->Submit(wnd.Gfx());
    }
    for (const auto& pChild : node->GetChildren())
    {
        if (pChild)
        {
            DrawNodeRecursive(gfx, pChild.get()); 
        }
    }

}

void App::ShowControlWindows()
{
	
    pointLight.Submit();
    dirLight.SpawnControlWindow();
    pointLight.SpawnControlWindow(); 
    if (showDemoWindow)
    {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    if (ImGui::Begin("Scene Hierarchy"))
    {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        if (pSceneRoot)
        {
            pSceneRoot->ShowNodeTree(pSelectedSceneNode);
        }

        ImGui::Separator();

        if (pSelectedSceneNode != nullptr)
        {
            ImGui::Text("Selected: %s", pSelectedSceneNode->GetName().c_str());
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {

                DirectX::XMFLOAT3 pos = pSelectedSceneNode->GetLocalPosition();
                DirectX::XMFLOAT3 rotRad = pSelectedSceneNode->GetLocalRotationEuler();
                DirectX::XMFLOAT3 scale = pSelectedSceneNode->GetLocalScale();


                DirectX::XMFLOAT3 rotDeg = {
                    DirectX::XMConvertToDegrees(rotRad.x),
                    DirectX::XMConvertToDegrees(rotRad.y),
                    DirectX::XMConvertToDegrees(rotRad.z)
                };

                bool transformChanged = false;

                static char posInputBuf[64] = {};
                snprintf(posInputBuf, sizeof(posInputBuf), "{%.1ff, %.1ff, %.1ff}", pos.x, pos.y, pos.z);
                if (ImGui::InputText("P", posInputBuf, sizeof(posInputBuf))) {
                    // Optional: parse the input and update pos if valid
                    float x, y, z;
                    if (sscanf_s(posInputBuf, "{%ff, %ff, %ff}", &x, &y, &z) == 3) {
                        pos = { x, y, z };
                        pSelectedSceneNode->SetLocalPosition(pos);
                        transformChanged = true;
                    }
                }
                ImGui::Text("Position"); ImGui::SameLine();
                if (ImGui::DragFloat3("##Position", &pos.x, 0.1f))
                {
                    pSelectedSceneNode->SetLocalPosition(pos);
                    transformChanged = true;
                }

                ImGui::Text("Rotation"); ImGui::SameLine();
                if (ImGui::DragFloat3("##Rotation", &rotDeg.x, 1.0f)) 
                {

                    rotRad = {
                        DirectX::XMConvertToRadians(rotDeg.x),
                        DirectX::XMConvertToRadians(rotDeg.y),
                        DirectX::XMConvertToRadians(rotDeg.z)
                    };
                    pSelectedSceneNode->SetLocalRotation(rotRad);
                    transformChanged = true;
                }

                ImGui::Text("Scale   "); ImGui::SameLine(); 
                if (ImGui::DragFloat3("##Scale", &scale.x, 0.01f, 0.01f, 100.0f)) 
                {

                    scale.x = std::max(scale.x, 0.001f);
                    scale.y = std::max(scale.y, 0.001f);
                    scale.z = std::max(scale.z, 0.001f);
                    pSelectedSceneNode->SetLocalScale(scale);
                    transformChanged = true;
                }


            }


            if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
            {
                const auto& components = pSelectedSceneNode->GetComponents();
                if (components.empty()) {
                    ImGui::TextDisabled("No components attached.");
                }
                else {
                    int compIndex = 0;
                    for (const auto& comp : components) {
                        if (comp) {
                           
                            std::string compLabel = typeid(*comp).name();

                            if (compLabel.rfind("class ", 0) == 0) {
                                compLabel = compLabel.substr(6);
                            }
                            compLabel += "##" + std::to_string(compIndex++); 


                            if (ImGui::TreeNode(compLabel.c_str()))
                            {

                                comp->DrawImGuiControls();


                                ImGui::TreePop();
                            }
                        }
                    }
                }
            }

        }
        else
        {
            ImGui::Text("Selected: None");
        }
    }
    ImGui::End(); 
}




void App::CleanupDestroyedNodes(Node* currentNode)
{
    if (!currentNode) return;

    //for (int i = 0; i < currentNode->GetChildren().size(); ++i) {
    //    CleanupDestroyedNodes(currentNode->GetChildren()[i].get());
    //}

    //if (currentNode->markedForDestruction)
    //{
    //    for (int i = 0; i < currentNode->GetChildren().size(); ++i)
    //    {
    //        Node* pChildNode = currentNode->GetChildren()[i].get();
    //        if (pSelectedSceneNode == pChildNode) { pSelectedSceneNode = nullptr; }
    //        if (pChildNode->GetComponent<Rigidbody>() != nullptr) 
    //        {
    //            dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->RemoveRigidbodyData(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
    //            //PhysicsCommon::physicsSystem->GetBodyInterface().DeactivateBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
    //            //PhysicsCommon::physicsSystem->GetBodyInterface().RemoveBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
    //        }
    //        if (pChildNode->GetComponent<Trigger>() != nullptr)
    //        {
    //            dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->RemoveTriggerData(pChildNode->GetComponent<Trigger>()->GetBodyID());
    //        }
    //        currentNode->RemoveChild(currentNode->GetChildren()[i].get());
    //    }
    //}
    //else
    //{
    //    return;
    //}
    //if (currentNode == pSceneRoot.get()) return;
    //if (!currentNode->GetParent()->markedForDestruction)
    //{
    //    if (pSelectedSceneNode == currentNode) { pSelectedSceneNode = nullptr; }
    //    currentNode->GetParent()->RemoveChild(currentNode);
    //}


    auto& children_ref = currentNode->GetChildren_NonConst();
    for (size_t i = 0; i < children_ref.size(); ++i) {
        if (children_ref[i]) {
            CleanupDestroyedNodes(children_ref[i].get());
        }
    }


    auto& children = currentNode->GetChildren_NonConst();
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [&](std::unique_ptr<Node>& pChildNode) {
                if (!pChildNode) return false;

                if (pChildNode->IsMarkedForDestruction()) {
                    OutputDebugStringA(("Cleanup: Preparing to remove node: " + pChildNode->GetName() + "\n").c_str());

                  /*  if (pChildNode->GetComponent<Rigidbody>() != nullptr) {
                        PhysicsCommon::physicsSystem->GetBodyInterface().DeactivateBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                        PhysicsCommon::physicsSystem->GetBodyInterface().RemoveBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                    }*/
                    if (pChildNode->GetComponent<Rigidbody>() != nullptr)
                    {
                        dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->RemoveRigidbodyData(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                        //PhysicsCommon::physicsSystem->GetBodyInterface().DeactivateBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                        //PhysicsCommon::physicsSystem->GetBodyInterface().RemoveBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                    }
                    if (pChildNode->GetComponent<Trigger>() != nullptr)
                    {
                    }

                    const auto& components = pChildNode->GetComponents();
                    for (const auto& compUniquePtr : components) {
                        Component* comp = compUniquePtr.get();
                        if (!comp) continue;


                      

                    }

                    if (pSelectedSceneNode == pChildNode.get()) { pSelectedSceneNode = nullptr; }
                    if (pPlayer == pChildNode.get()) { pPlayer = nullptr; }

                    OutputDebugStringA(("Cleanup: Node removed from parent: " + pChildNode->GetName() + "\n").c_str());
                    return true;
                }
                return false;
            }
        ),
        children.end()
    );

    
}

void App::SaveNodeTransformsRecursive(Node& node, std::ofstream& file)
{
    // Pomi� zapisywanie w�z��w "technicznych" lub pustych, je�li chcesz
    if (node.GetName() != "Root" && node.GetName() != "L Normal" && node.GetName() != "Camera" && node.GetName() != "FreeViewCamera" &&
        node.GetName() != "L Ability" && node.GetName() != "R Normal" && node.GetName() != "R Ability" && node.GetName() != "Player")
    {
        DirectX::XMFLOAT3 pos = node.GetLocalPosition();
        DirectX::XMFLOAT3 rot = node.GetLocalRotationEuler();

        // Ustaw precyzj� zapisu, aby unikn�� notacji naukowej i uzyska� czytelne liczby
        file << std::fixed << std::setprecision(2);

        // Zapisz nazw� i pozycj� w jednej linii
        file << "Object: " << std::setw(20) << std::left << node.GetName()
            << " Position: {" << pos.x << "f, " << pos.y << "f, " << pos.z << "f}" 
			<< " Rotation: {" << rot.x << "f, " << rot.y << "f, " << rot.z << "f}"
            << std::endl;
    }

    // Wywo�aj rekurencyjnie dla wszystkich dzieci tego w�z�a
    for (const auto& pChild : node.GetChildren())
    {
        if (pChild)
        {
            SaveNodeTransformsRecursive(*pChild, file);
        }
    }
}
