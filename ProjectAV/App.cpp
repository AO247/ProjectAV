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
#include "SoundEffectsLibrary.h"
#include "SoundEffectsPlayer.h"
#include "DebugLine.h"
#include "Testing.h"
#include "PrefabManager.h"
#include "LevelGenerator.h"
#include "WindTunnelEffect.h"
#include <Jolt/Jolt.h>
#include <Jolt/ConfigurationString.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include "imgui/imgui_impl_dx11.h"

namespace dx = DirectX;

App::App(const std::string& commandLine)
    :
    commandLine(commandLine),
    wnd(1920, 1080, "Project AV"), // Pass window dimensions/title
    scriptCommander(TokenizeQuoted(commandLine)),
    pointLight(wnd.Gfx(), 2u), // Initialize PointLight
    dirLight(wnd.Gfx(), 0u),
    pSceneRoot(std::make_unique<Node>("Root"))
{
    // Set Projection Matrix (Far plane adjusted for larger scenes potentially)
    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.5f, 2000.0f));


    std::srand(static_cast<unsigned>(std::time(nullptr)));
    pointLight.LinkTechniques(rg);

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

    /*physicsDebugRenderer = new PhysicsDebugRenderer(wnd.Gfx());
    physicsDebugRenderer->Initialize();*/

    soundDevice = LISTENER->Get();
    ALint attentuation = AL_INVERSE_DISTANCE_CLAMPED;

	soundDevice->SetAttenuation(attentuation);
    myMusic = std::make_unique<MusicBuffer>("Music\\windererfull.mp3");
    myMusic->setGain(0.2f);

    // --- Create Nodes ---

    auto pCameraNodeOwner = std::make_unique<Node>("Camera", nullptr, "CAMERA");
    pCamera = pCameraNodeOwner.get();
    auto pFreeViewCameraOwner = std::make_unique<Node>("FreeViewCamera");
    pFreeViewCamera = pFreeViewCameraOwner.get();
    auto pPlayerOwner = std::make_unique<Node>("Player", nullptr, "PLAYER");
    pPlayer = pPlayerOwner.get();
    auto pAbility1Owner = std::make_unique<Node>("Ability1", nullptr, "TRIGGER");
    pAbility1 = pAbility1Owner.get();
    auto pAbility2Owner = std::make_unique<Node>("Ability2", nullptr, "TRIGGER");
    pAbility2 = pAbility2Owner.get();
    auto pAbility3Owner = std::make_unique<Node>("Ability3", nullptr, "TRIGGER");
    pAbility3 = pAbility3Owner.get();
    auto pAbility4Owner = std::make_unique<Node>("Ability4", nullptr, "TRIGGER");
    pAbility4 = pAbility4Owner.get();
    auto pPrefabsOwner = std::make_unique<Node>("Prefabs", nullptr, "PREFABS");
    pPrefabs = pPrefabsOwner.get();
    auto pLeftHandNormalOwner = std::make_unique<Node>("L Normal", nullptr, "HANDS");
    pLeftHandNormal = pLeftHandNormalOwner.get();
    auto pLeftHandAbilityOwner = std::make_unique<Node>("L Ability", nullptr, "HANDS");
    pLeftHandAbility = pLeftHandAbilityOwner.get();
    auto pRightHandNormalOwner = std::make_unique<Node>("R Normal", nullptr, "HANDS");
    pRightHandNormal = pRightHandNormalOwner.get();
    auto pRightHandAbilityOwner = std::make_unique<Node>("R Ability", nullptr, "HANDS");
    pRightHandAbility = pRightHandAbilityOwner.get();





    // Adding to Scene Graph
    pSceneRoot->AddChild(std::move(pCameraNodeOwner));
    pSceneRoot->AddChild(std::move(pFreeViewCameraOwner));
    pSceneRoot->AddChild(std::move(pPlayerOwner));
    pSceneRoot->AddChild(std::move(pAbility1Owner));
    pSceneRoot->AddChild(std::move(pAbility2Owner));
    pSceneRoot->AddChild(std::move(pAbility3Owner));
    pSceneRoot->AddChild(std::move(pAbility4Owner));
    pSceneRoot->AddChild(std::move(pPrefabsOwner));
    pCamera->AddChild(std::move(pLeftHandNormalOwner));
    pCamera->AddChild(std::move(pLeftHandAbilityOwner));
    pCamera->AddChild(std::move(pRightHandNormalOwner));
    pCamera->AddChild(std::move(pRightHandAbilityOwner));


    PrefabManager::root = pPrefabs;
    PrefabManager::player = pPlayer;

    //Heeeej Bracie zaczlooo padac choodz zmieniic gacieee
    //Heeeej Siostro uciekaajmyy zanim beedzieee mookroooo

    BodyCreationSettings bodySettings(new JPH::CapsuleShape(1.0f, 1.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::PLAYER);
    bodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    //bodySettings.mMassPropertiesOverride.SetMassAndInertiaOfSolidBox(Vec3(2.0f, 4.0f, 2.0f), 10.0f);
    bodySettings.mMassPropertiesOverride.mMass = 1.0f;
    bodySettings.mFriction = 0.0f;
    bodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
    bodySettings.mMotionQuality = EMotionQuality::LinearCast;
    pPlayer->AddComponent(
        std::make_unique<Rigidbody>(pPlayer, bodySettings)
    );
    Rigidbody* pRigidbody = pPlayer->GetComponent<Rigidbody>();

    pPlayer->AddComponent(
        std::make_unique<PlayerController>(pPlayer, wnd) // Add controller first
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
    /* pAbility2->AddComponent(
         std::make_unique<ModelComponent>(pAbility2, wnd.Gfx(), "Models\\box.glb")
     );*/
     //pAbility2->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pPlayer->GetComponent<PlayerController>()->abilitySlot2 = pAbility2;


    BodyCreationSettings a3odySettings(new JPH::SphereShape(40.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility3->AddComponent(
        std::make_unique<Trigger>(pAbility3, a3odySettings, false)
    );
    pAbility3->AddComponent(
        std::make_unique<Ability3>(pAbility3, wnd, pCamera)
    );
    /* pAbility3->AddComponent(
         std::make_unique<ModelComponent>(pAbility3, wnd.Gfx(), "Models\\box.glb")
     );
     pAbility3->GetComponent<ModelComponent>()->LinkTechniques(rg);*/
    pPlayer->GetComponent<PlayerController>()->abilitySlot3 = pAbility3;


    BodyCreationSettings a4odySettings(new JPH::SphereShape(2.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility4->AddComponent(
        std::make_unique<Trigger>(pAbility4, a4odySettings, false)
    );
    /* pAbility4->AddComponent(
         std::make_unique<ModelComponent>(pAbility4, wnd.Gfx(), "Models\\box.glb")
     );
     pAbility4->GetComponent<ModelComponent>()->LinkTechniques(rg);*/
    pAbility4->AddComponent(
        std::make_unique<Ability4>(pAbility4, wnd, pCamera)
    );

    // pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility4;

     //Adding Other Components
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
    SoundEffectsPlayer* pSoundEffectsPlayer = pPlayer->GetComponent<SoundEffectsPlayer>();
    pSoundEffectsPlayer->AddSound("Models\\turn.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\push1.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\push2.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\toss1.ogg");
    pSoundEffectsPlayer->AddSound("Sounds\\toss2.ogg");
    //pSoundEffectsPlayer->AddSound("Models\\sci-fidrone.ogg");

    // Changing position scale etc.]
    pFreeViewCamera->SetLocalPosition({ 4.0f, 11.0f, -28.0f });
    pPlayer->SetLocalPosition({ 0.0f, 80.0f, -24.0f });

    //pEnemySoundEffectsPlayer->SetPosition(0.0f, 0.0f, 0.0f);
    //soundDevice->SetLocation(pPlayer->GetLocalPosition().x, pPlayer->GetLocalPosition().y, pPlayer->GetLocalPosition().z);

    pSceneRoot->AddComponent(
        std::make_unique<Global>(pSceneRoot.get(), wnd, pPlayer)
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

    // --- Prefabs ---

    //pEnemy->AddComponent(
    //	std::make_unique<SoundEffectsPlayer>(pEnemy)
    //);
    //SoundEffectsPlayer* pEnemySoundEffectsPlayer = pEnemy->GetComponent<SoundEffectsPlayer>();
    //pEnemySoundEffectsPlayer->AddSound("Models\\sci-fidrone.ogg");
    pSceneRoot->AddComponent(
        std::make_unique<UpgradeHandler>(pSceneRoot.get(), wnd)
    );
    pUpgradeHandler = pSceneRoot->GetComponent<UpgradeHandler>();
    pUpgradeHandler->ability1Node = pAbility1;
    pUpgradeHandler->ability2Node = pAbility2;
    pUpgradeHandler->ability3Node = pAbility3;
    pUpgradeHandler->ability4Node = pAbility4;
    pUpgradeHandler->playerController = pPlayer->GetComponent<PlayerController>();
    pUpgradeHandler->SetBasicValues();
    pSceneRoot->GetComponent<Global>()->upgradeHandler = pUpgradeHandler;
    //LevelGenerator levelGenerator(prefabManager, pSceneRoot.get(), pPlayer);


    const int screenWidth = 1920;
    const int screenHeight = 1080;
    const int plusSpriteWidth = 32;
    const int plusSpriteHeight = 32;
    const int plusSpriteX = (screenWidth / 2) - (plusSpriteWidth / 2);
    const int plusSpriteY = (screenHeight / 2) - (plusSpriteHeight / 2);

    targetSprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        plusSpriteX,                // int x (center X)
        plusSpriteY,                // int y (center Y)
        plusSpriteWidth,            // int width
        plusSpriteHeight,           // int height
        L"Images\\plus.png"
    );

    heart1Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        (screenWidth / 2) - 35 - 80,                // int x  
        950,                // int y  
        70,            // int width
        70,           // int height
        L"Images\\heart.png"
    );

    heart2Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        (screenWidth / 2) - 35,                // int x  
        950,                // int y (center Y)
        70,            // int width
        70,           // int height
        L"Images\\heart.png"
    );

    heart3Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),      // ID3D11Device*
        (screenWidth / 2) - 35 + 80,                // int x (center X)
        950,                // int y (center Y)
        70,            // int width
        70,           // int height
        L"Images\\heart.png"
    );



    wnd.DisableCursor();
    wnd.mouse.EnableRaw();
    cursorEnabled = false;

}

App::~App()
{
    //for (auto& pair : sphereCollidersToDraw) {
    //    delete pair.second; // Delete the ColliderSphere*
    //}
    //sphereCollidersToDraw.clear();

    //for (auto& pair : boxCollidersToDraw) {
    //    delete pair.second; // Delete the SolidBox*
    //}
    //boxCollidersToDraw.clear();

    //for (auto& pair : capsuleCollidersToDraw) {
    //    delete pair.second; // Delete the SolidCapsule*
    //}
    //capsuleCollidersToDraw.clear();

    // Delete the member DebugLine pointers
    /*delete line1; line1 = nullptr;
    delete line2; line2 = nullptr;
    delete line3; line3 = nullptr;
    delete line4; line4 = nullptr;*/
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


// --- UPDATED HandleInput ---
void App::HandleInput(float dt)
{

    // --- Only handle non-player input here ---
    while (const auto e = wnd.kbd.ReadKey())
    {
        if (!e->IsPress()) continue;

        switch (e->GetCode())
        {
        case 'O':
        {
            pSceneRoot->GetComponent<Global>()->AddSpecialLevel();
            break;
        }

		case 'M': // Toggle Music
			if (myMusic->isPlaying())
			{
				myMusic->Stop();
			}
			else
			{
				myMusic->Play();
			}
			break;
        case 'C': // Toggle cursor
            if (cursorEnabled) {
                wnd.DisableCursor();
                wnd.mouse.EnableRaw();
            }
            else {
                wnd.EnableCursor();
                wnd.mouse.DisableRaw();
            }
            cursorEnabled = !cursorEnabled;
            break;
        case 'H': // Toggle UI
            showControlWindow = !showControlWindow;
            break;
        case 'B':
            if (pPlayer->GetComponent<PlayerController>()->abilitySlot1 == pAbility1)
            {
                pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility4;
            }
            else
            {
                pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility1;
            }
            break;
        case VK_ESCAPE: // Exit
            PostQuitMessage(0);
            return;
        case VK_F1: // Toggle ImGui Demo
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

    // FreeCamera Movement
    if (freeViewCamera) {
        if (wnd.kbd.KeyIsPressed('I')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.0f, 0.4f });
        }
        if (wnd.kbd.KeyIsPressed('K')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.0f, -0.4f });
        }
        if (wnd.kbd.KeyIsPressed('J')) {
            pFreeViewCamera->TranslateLocal({ -0.4f, 0.0f, 0.0f });
        }
        if (wnd.kbd.KeyIsPressed('L')) {
            pFreeViewCamera->TranslateLocal({ 0.4f, 0.0f, 0.0f });
        }
        if (wnd.kbd.KeyIsPressed('U')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, -0.4f, 0.0f });
        }
        if (wnd.kbd.KeyIsPressed('O')) {
            pFreeViewCamera->TranslateLocal({ 0.0f, 0.4f, 0.0f });
        }
    }
}

void App::DoFrame(float dt)
{
	pSceneRoot->Update(dt); // Update the scene root and all its children
    auto* contact = dynamic_cast<MyContactListener*>(physicsSystem->GetContactListener());
    contact->ExecuteTriggerActivationQueue();
    contact->ExecuteCollisionActivationQueue();
    CleanupDestroyedNodes(pSceneRoot.get());

    wnd.Gfx().BeginFrame(0.5f, 0.5f, 1.0f);
    //if (pPlayer->GetLocalPosition().y < -10.0f) {
    //	pPlayer->SetLocalPosition({ -20.0f, 225.0f, -25.0f });
    //    pEnemy->SetLocalPosition({ 15.0f, 225.0f, 0.0f });
    //}
    dx::XMMATRIX viewMatrix = pCamera->GetComponent<Camera>()->GetViewMatrix();
    if (freeViewCamera)
    {
        viewMatrix = pFreeViewCamera->GetComponent<Camera>()->GetViewMatrix();
    }
    wnd.Gfx().SetCamera(viewMatrix);
    //fc.ShowWindows(wnd.Gfx());
	/*DebugLine line(wnd.Gfx(), pEnemy->GetComponent<StateMachine>()->pos, pEnemy->GetComponent<StateMachine>()->cen, { 0.0f, 0.0f, 1.0f, 1.0f });
    line.Submit(fc);*/ // for idle
    // --- Bind Lights ---

    pointLight.Bind(wnd.Gfx(), viewMatrix);
    dirLight.Bind(wnd.Gfx(), viewMatrix);

    FrustumCalculating(); // Draw with FRUSTUM CULLING
    //pSceneRoot->Submit(fc, wnd.Gfx()); // Draw without FRUSTUM CULLING you have to also uncomment the draw method in Node.cpp



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
	//pCamera->Forward();

    rg.Execute(wnd.Gfx());

    if (showControlWindow) {
        ShowControlWindows();
    }

    
    pUpgradeHandler->DrawUpgradeMenu();

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


    //testButton->Draw(wnd.Gfx().GetContext(), (float)wnd.GetWidth(), (float)wnd.GetHeight());

    wnd.Gfx().EndFrame();
    rg.Reset();
}


void App::FrustumCalculating() {
    dx::XMMATRIX camWorldTransform = pCamera->GetWorldTransform();
    dx::XMVECTOR camWorldPos = camWorldTransform.r[3]; // Default
    dx::XMVECTOR camWorldForward = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), camWorldTransform)); // Default
    dx::XMVECTOR camWorldUp = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), camWorldTransform));    // Default
    dx::XMVECTOR camWorldRight = dx::XMVector3Normalize(dx::XMVector3TransformNormal(dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), camWorldTransform)); // Default

    constexpr float fovAngleY = DirectX::XMConvertToRadians(70.0f); // Field of View in Y direction (radians) - MUST MATCH YOUR PROJECTION
    const float aspectRatio = 16.0f / 9.0f; // MUST MATCH YOUR PROJECTION
    const float nearDist = 0.5f;         // MUST MATCH YOUR PROJECTION
    const float farDist = 800.0f;       // MUST MATCH YOUR PROJECTION

    // Half heights/widths at near and far planes
    float halfHeightNear = nearDist * tanf(fovAngleY * 0.5f);
    float halfWidthNear = halfHeightNear * aspectRatio;
    float halfHeightFar = farDist * tanf(fovAngleY * 0.5f);
    float halfWidthFar = halfHeightFar * aspectRatio;

    dx::XMVECTOR nearCenter = dx::XMVectorAdd(camWorldPos, dx::XMVectorScale(camWorldForward, nearDist));
    dx::XMVECTOR farCenter = dx::XMVectorAdd(camWorldPos, dx::XMVectorScale(camWorldForward, farDist));

    // Calculate the 8 corner points (optional for plane calculation, but useful for verification)
    dx::XMVECTOR nearTopLeft = dx::XMVectorAdd(nearCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearTopRight = dx::XMVectorAdd(nearCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearBottomLeft = dx::XMVectorAdd(nearCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, -halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR nearBottomRight = dx::XMVectorAdd(nearCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, -halfHeightNear), dx::XMVectorScale(camWorldRight, halfWidthNear)));
    dx::XMVECTOR farTopLeft = dx::XMVectorAdd(farCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farTopRight = dx::XMVectorAdd(farCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farBottomLeft = dx::XMVectorAdd(farCenter, dx::XMVectorSubtract(dx::XMVectorScale(camWorldUp, -halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));
    dx::XMVECTOR farBottomRight = dx::XMVectorAdd(farCenter, dx::XMVectorAdd(dx::XMVectorScale(camWorldUp, -halfHeightFar), dx::XMVectorScale(camWorldRight, halfWidthFar)));

    cameraFrustum.Near = 0.5f;

    // Far Plane: Normal = -camWorldForward, Point = farCenter
    cameraFrustum.Far = 800.0f;

    float tanHalfFovY = tanf(fovAngleY * 0.5f);
    cameraFrustum.TopSlope = tanHalfFovY;
    cameraFrustum.BottomSlope = -tanHalfFovY;

    cameraFrustum.RightSlope = tanHalfFovY * aspectRatio;
    cameraFrustum.LeftSlope = -tanHalfFovY * aspectRatio;

    DirectX::XMStoreFloat3(&cameraFrustum.Origin, camWorldPos);
    dx::XMMATRIX worldOrientationMatrix;
    worldOrientationMatrix.r[0] = camWorldRight;   // World X-axis of the camera
    worldOrientationMatrix.r[1] = camWorldUp;      // World Y-axis of the camera
    worldOrientationMatrix.r[2] = camWorldForward; // World Z-axis of the camera
    worldOrientationMatrix.r[3] = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Set W component for a valid matrix

    // 3. Convert the Orientation Matrix to a Quaternion
    DirectX::XMStoreFloat4(&cameraFrustum.Orientation, dx::XMQuaternionRotationMatrix(worldOrientationMatrix));


    DrawNodeRecursive(wnd.Gfx(), *pSceneRoot);
}

void App::DrawNodeRecursive(Graphics& gfx, Node& node)
{
    // --- Culling Check ---
    bool shouldDraw = true; // Assume we draw by default
    ModelComponent* modelComp = node.GetComponent<ModelComponent>();

    if (modelComp != nullptr) // Only cull nodes with models (or add BoundsComponent later)
    {
        DirectX::BoundingSphere sphere;
        DirectX::BoundingBox box;
        DirectX::ContainmentType containment = DirectX::DISJOINT;
        sphere.Center = node.GetWorldPosition();
        sphere.Radius = node.radius;
        containment = cameraFrustum.Contains(sphere);
        //if (node.GetComponent<BoundingSphere>() != nullptr)
        //{
        //    sphere.Center = node.GetWorldPosition();
        //    sphere.Radius = node.GetComponent<BoundingSphere>()->GetRadius();
        //    containment = cameraFrustum.Contains(sphere);
        //}
        //else if (node.GetComponent<OBB>() != nullptr)
        //{
        //    box.Center = node.GetWorldPosition();
        //    box.Extents = node.GetComponent<OBB>()->GetTransformedSize();
        //    containment = cameraFrustum.Contains(box);
        //}
        //else if (node.GetComponent<CapsuleCollider>() != nullptr)
        //{
        //    // Assuming you have a method to get the capsule's bounding sphere
        //    sphere.Center = DirectX::XMFLOAT3(node.GetWorldPosition().x,
        //        (node.GetWorldPosition().y + 1.5f),
        //        node.GetWorldPosition().z);
        //    sphere.Radius = node.GetComponent<CapsuleCollider>()->GetRadius() * 2.5f;
        //    containment = cameraFrustum.Contains(sphere);
        //}

        if (containment == DirectX::DISJOINT) // DISJOINT means completely outside
        {
            shouldDraw = false; // Don't draw this node or its children
        }

    }

    if (shouldDraw)
    {
        node.Submit(wnd.Gfx());
        for (const auto& pChild : node.GetChildren())
        {
            if (pChild)
            {
                DrawNodeRecursive(gfx, *pChild); // Recurse for children
            }
        }
    }
}

void App::ShowControlWindows()
{
	//if (!showControlWindow) return; // If control window is not enabled, exit
    // --- Existing Windows ---
	//DrawSphereColliders(wnd.Gfx()); // Call the updated function
    //DrawBoxColliders(wnd.Gfx()); // Call the updated function
	//DrawCapsuleColliders(wnd.Gfx());
    //ForEnemyWalking();
    pointLight.Submit();
    dirLight.SpawnControlWindow();
    pointLight.SpawnControlWindow(); // Control for Point Light
    if (showDemoWindow)
    {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    // --- Show Model Component Windows ---
    //if (pNanosuitNode)
    //{
    //    if (auto* modelComp = pNanosuitNode->GetComponent<ModelComponent>())
    //    {
    //        modelComp->ShowWindow("Nanosuit Controls");
    //    }
    //}


    // --- NEW: Scene Hierarchy Window ---
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

            // --- Transform Editor ---
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Get current values
                DirectX::XMFLOAT3 pos = pSelectedSceneNode->GetLocalPosition();
                DirectX::XMFLOAT3 rotRad = pSelectedSceneNode->GetLocalRotationEuler();
                DirectX::XMFLOAT3 scale = pSelectedSceneNode->GetLocalScale();

                // Convert rotation to degrees for easier editing
                DirectX::XMFLOAT3 rotDeg = {
                    DirectX::XMConvertToDegrees(rotRad.x),
                    DirectX::XMConvertToDegrees(rotRad.y),
                    DirectX::XMConvertToDegrees(rotRad.z)
                };

                bool transformChanged = false;

                ImGui::Text("Position"); ImGui::SameLine();
                if (ImGui::DragFloat3("##Position", &pos.x, 0.1f))
                {
                    pSelectedSceneNode->SetLocalPosition(pos);
                    transformChanged = true;
                }

                ImGui::Text("Rotation"); ImGui::SameLine();
                if (ImGui::DragFloat3("##Rotation", &rotDeg.x, 1.0f)) // Edit degrees
                {
                    // Convert back to radians before setting
                    rotRad = {
                        DirectX::XMConvertToRadians(rotDeg.x),
                        DirectX::XMConvertToRadians(rotDeg.y),
                        DirectX::XMConvertToRadians(rotDeg.z)
                    };
                    pSelectedSceneNode->SetLocalRotation(rotRad); // Set radians
                    transformChanged = true;
                }

                ImGui::Text("Scale   "); ImGui::SameLine(); // Extra spaces for alignment
                if (ImGui::DragFloat3("##Scale", &scale.x, 0.01f, 0.01f, 100.0f)) // Add min/max scale
                {
                    // Prevent zero scale if needed
                    scale.x = std::max(scale.x, 0.001f);
                    scale.y = std::max(scale.y, 0.001f);
                    scale.z = std::max(scale.z, 0.001f);
                    pSelectedSceneNode->SetLocalScale(scale);
                    transformChanged = true;
                }

                // Note: The Node's worldTransformDirty flag is already set by the SetLocal... methods.
                // The Node::Update() call later will handle recalculating the world matrix.
            }

            // --- Component Viewer ---
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
                            // Create a unique ID for the component header
                            std::string compLabel = typeid(*comp).name();
                            // Remove "class " prefix if present (platform dependent)
                            if (compLabel.rfind("class ", 0) == 0) {
                                compLabel = compLabel.substr(6);
                            }
                            compLabel += "##" + std::to_string(compIndex++); // Add unique ID

                            // Make each component collapsible
                            if (ImGui::TreeNode(compLabel.c_str()))
                            {
                                // --- Call the component's ImGui draw function ---
                                comp->DrawImGuiControls();
                                // --- End Call ---

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
    ImGui::End(); // End Scene Hierarchy Window
}




void App::CleanupDestroyedNodes(Node* currentNode)
{
    if (!currentNode) return;

    // 1. Recursively clean children first.
    auto& children_ref = currentNode->GetChildren_NonConst();
    for (size_t i = 0; i < children_ref.size(); ++i) {
        if (children_ref[i]) {
            CleanupDestroyedNodes(children_ref[i].get());
        }
    }

    // 2. Now, remove any children of *this* currentNode that are marked.
    auto& children = currentNode->GetChildren_NonConst();
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [&](std::unique_ptr<Node>& pChildNode) {
                if (!pChildNode) return false;

                if (pChildNode->IsMarkedForDestruction()) {
                    OutputDebugStringA(("Cleanup: Preparing to remove node: " + pChildNode->GetName() + "\n").c_str());

                    if (pChildNode->GetComponent<Rigidbody>() != nullptr) {
                        PhysicsCommon::physicsSystem->GetBodyInterface().DeactivateBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                        PhysicsCommon::physicsSystem->GetBodyInterface().RemoveBody(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                        /*if (pChildNode->GetComponent<Trigger>() != nullptr) {
                            dynamic_cast<MyContactListener*>(PhysicsCommon::physicsSystem->GetContactListener())->RemoveRigidbodyData(pChildNode->GetComponent<Rigidbody>()->GetBodyID());
                        }*/
                    }


                    const auto& components = pChildNode->GetComponents();
                    for (const auto& compUniquePtr : components) {
                        Component* comp = compUniquePtr.get();
                        if (!comp) continue;
                        //physicsEngine.RemoveCollider(dynamic_cast<Collider*>(comp));


                        // FOR DRAWING COLLIDERS
                        //if (auto* bs = dynamic_cast<BoundingSphere*>(comp)) {
                        //    physicsEngine.RemoveCollider(bs); // Remove from physics
                        //    auto mapIt = sphereCollidersToDraw.find(bs);
                        //    if (mapIt != sphereCollidersToDraw.end()) {
                        //        delete mapIt->second;
                        //        sphereCollidersToDraw.erase(mapIt);
                        //    }
                        //}
                        //else if (auto* obb = dynamic_cast<OBB*>(comp)) {
                        //    auto mapIt = boxCollidersToDraw.find(obb);
                        //    if (mapIt != boxCollidersToDraw.end()) {
                        //        delete mapIt->second;
                        //        boxCollidersToDraw.erase(mapIt);
                        //    }
                        //}
                        //else if (auto* cap = dynamic_cast<CapsuleCollider*>(comp)) {
                        //    auto mapIt = capsuleCollidersToDraw.find(cap);
                        //    if (mapIt != capsuleCollidersToDraw.end()) {
                        //        delete mapIt->second; // Delete the allocated SolidCapsule* drawable
                        //        capsuleCollidersToDraw.erase(mapIt);
                        //    }
                        //}
						// END FOR DRAWING COLLIDERS

                    }

                    if (pSelectedSceneNode == pChildNode.get()) { pSelectedSceneNode = nullptr; }
                    if (pPlayer == pChildNode.get()) { pPlayer = nullptr; }
                    if (pAbility1 == pChildNode.get()) { pAbility1 = nullptr; }
                    if (pAbility2 == pChildNode.get()) { pAbility2 = nullptr; }

                    OutputDebugStringA(("Cleanup: Node removed from parent: " + pChildNode->GetName() + "\n").c_str());
                    return true;
                }
                return false;
            }
        ),
        children.end()
    );

    
}
