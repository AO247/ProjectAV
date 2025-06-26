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
#include "TransformCbuf.h"
#include "ShadowCbuf.h"
namespace dx = DirectX;

App::App(const std::string& commandLine)
    :
    commandLine(commandLine),
    wnd(1920, 1080, "Winderer"), 
    pointLight(wnd.Gfx(), 2u), 
    dirLight(wnd.Gfx(), 0u),
    pSceneRoot(std::make_unique<Node>("Root"))
{

    wnd.Gfx().SetProjection(dx::XMMatrixPerspectiveLH(1.0f, 9.0f / 16.0f, 0.3f, 2000.0f));


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
    myMusic->setGain(0.05f);

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
    auto pTemporaryOwner = std::make_unique<Node>("Temporary", nullptr, "TEMPORARY");
    temporary = pTemporaryOwner.get();
    auto pLeftHandOwner = std::make_unique<Node>("L Normal", nullptr, "HANDS");
    pLeftHand = pLeftHandOwner.get();
    auto pRightHandOwner = std::make_unique<Node>("R Normal", nullptr, "HANDS");
    pRightHand = pRightHandOwner.get();
	auto handsOwner = std::make_unique<Node>("Hands", nullptr, "HANDS");
    auto tutorialOwner = std::make_unique<Node>("Tutorial", nullptr, "TUTORIAL");
 
    tutorialNode = tutorialOwner.get();
	Node* pPlayerThings = playerThings.get();
	Node* pAbilities = abilities.get();
	Node* pBase = base.get();
	Node* pHands = handsOwner.get(); 
    pSceneRoot->AddChild(std::move(pTemporaryOwner));
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
	pCamera->AddChild(std::move(handsOwner));
    pHands->AddChild(std::move(pLeftHandOwner));
    pHands->AddChild(std::move(pRightHandOwner));
    pSceneRoot->AddChild(std::move(tutorialOwner));

    //PrefabManager::InstantiateStone1(pSceneRoot.get(), Vector3(0.0f, 100.0f, 0.0f), 1.0f);

    PrefabManager::root = temporary;
    PrefabManager::player = pPlayer;
     

    BodyCreationSettings bodySettings(new JPH::CapsuleShape(1.4f, 1.6f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::PLAYER);
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
    pPlayer->GetComponent<PlayerController>()->leftHand = pLeftHand;
    pPlayer->GetComponent<PlayerController>()->rightHand = pRightHand;

    auto legsOwner = std::make_unique<Node>("Legs", nullptr, "TRIGGER");
    BodyCreationSettings p1TBodySettings(new JPH::SphereShape(12.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    legsOwner->AddComponent(
        std::make_unique<Trigger>(legsOwner.get(), p1TBodySettings, false)
    );
    legsOwner->AddComponent(
        std::make_unique<FallDamage>(legsOwner.get(), pPlayer)
    );
    legsOwner->SetLocalPosition({ 0.0f, -pPlayer->GetComponent<PlayerController>()->height / 2, 0.0f });
    pPlayer->AddChild(std::move(legsOwner));


    pHands->AddComponent(
        std::make_unique<Hands>(pHands)
    );
    pHands->GetComponent<Hands>()->playerController = pPlayer->GetComponent<PlayerController>();
	pHands->GetComponent<Hands>()->rigidbody = pPlayer->GetComponent<Rigidbody>();
    pHands->GetComponent<Hands>()->leftHand = pLeftHand;
    pHands->GetComponent<Hands>()->rightHand = pRightHand;
    pHands->GetComponent<Hands>()->cameraNode = pCamera;

    BodyCreationSettings a1BodySettings(new JPH::CapsuleShape(6.0f, 5.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility1->AddComponent(
        std::make_unique<Trigger>(pAbility1, a1BodySettings, false)
    );
    pAbility1->AddComponent(
        std::make_unique<Ability1>(pAbility1, wnd, pCamera)
    );
    pAbility1->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pAbility1)
    );
	pAbility1->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\push1.wav");
    pAbility1->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\push2.wav");
    pAbility1->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\push3.wav");
    pAbility1->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\push4.wav");
    pAbility1->SetLocalPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 10.0f));
    pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility1;
   

    BodyCreationSettings a2odySettings(new JPH::SphereShape(5.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility2->AddComponent(
        std::make_unique<Trigger>(pAbility2, a2odySettings, false)
    );
    pAbility2->AddComponent(
        std::make_unique<Ability2>(pAbility2, wnd, pCamera)
    );
    pAbility2->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pAbility2)
    );
    pAbility2->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss1.wav");
    pAbility2->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss2.wav");
    pAbility2->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss3.wav");
    pAbility2->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss4.wav");
    pPlayer->GetComponent<PlayerController>()->abilitySlot2 = pAbility2;



    BodyCreationSettings a3RbodySettings(new JPH::SphereShape(0.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Dynamic, Layers::TRIGGER);
    a3RbodySettings.mGravityFactor = 0.0f;
    a3RbodySettings.mOverrideMassProperties = EOverrideMassProperties::MassAndInertiaProvided;

    a3RbodySettings.mMassPropertiesOverride.mMass = 1.0f;
    a3RbodySettings.mFriction = 0.0f;
    a3RbodySettings.mAllowedDOFs = EAllowedDOFs::TranslationX | EAllowedDOFs::TranslationY | EAllowedDOFs::TranslationZ;
    a3RbodySettings.mMotionQuality = EMotionQuality::LinearCast;

    pAbility3->AddComponent(
        std::make_unique<Rigidbody>(pAbility3, a3RbodySettings)
    );

    BodyCreationSettings a3odySettings(new JPH::SphereShape(40.0f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility3->AddComponent(
        std::make_unique<Trigger>(pAbility3, a3odySettings, false)
    );
    pAbility3->AddComponent(
        std::make_unique<Ability3>(pAbility3, wnd, pCamera)
    );
    pAbility3->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pAbility3)
    );
    pAbility3->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\black_hole_start.wav");
    pAbility3->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\black_hole_throw.wav");
    pAbility3->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\black_hole_end.wav");
    pAbility3->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\hold.wav");
    pPlayer->GetComponent<PlayerController>()->abilitySlot3 = pAbility3;


    BodyCreationSettings a4odySettings(new JPH::SphereShape(2.5f), RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Kinematic, Layers::TRIGGER);
    pAbility4->AddComponent(
        std::make_unique<Trigger>(pAbility4, a4odySettings, false)
    );

    pAbility4->AddComponent(
        std::make_unique<Ability4>(pAbility4, wnd, pCamera)
    );
    pAbility4->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pAbility4)
    );
    pAbility4->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\sznurek1.wav");
    pAbility4->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\sznurek2.wav");
    pAbility4->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\hold.wav");
    pAbility4->GetComponent<Ability4>()->baseAbility = pAbility1->GetComponent<Ability1>();
    //pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility4;


    pAbility5->AddComponent(
        std::make_unique<Ability5>(pAbility5, wnd, pCamera)
    );
    pAbility5->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pAbility5)
    );
    pAbility5->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss1.wav");
    pAbility5->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss2.wav");
    pAbility5->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss3.wav");
    pAbility5->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\toss4.wav");
    pAbility5->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\hold.wav");
	//pPlayer->GetComponent<PlayerController>()->abilitySlot2 = pAbility5;

    pAbility6->AddComponent(
        std::make_unique<Ability6>(pAbility6, wnd, pCamera)
    );
    pAbility6->AddComponent(
        std::make_unique<SoundEffectsPlayer>(pAbility6)
    );
    pAbility6->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\gravity1.wav");
    pAbility6->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\gravity2.wav");
    pAbility6->GetComponent<SoundEffectsPlayer>()->AddSound("Sounds\\player\\hold.wav");
    pAbility6->GetComponent<Ability6>()->baseAbility = pAbility1->GetComponent<Ability1>();
    pPlayer->GetComponent<PlayerController>()->abilitySlot1 = pAbility6;

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
    pSoundEffectsPlayer->AddSound("Sounds\\player\\jump1.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\jump2.wav"); // 2
    pSoundEffectsPlayer->AddSound("Sounds\\player\\double1.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\double2.wav"); // 4
    pSoundEffectsPlayer->AddSound("Sounds\\player\\dash1.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\dash2.wav"); // 6
    pSoundEffectsPlayer->AddSound("Sounds\\player\\damage1.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\damage2.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\damage3.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\damage4.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\damage5.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\player\\damage6.wav"); // 12
    pSoundEffectsPlayer->AddSound("Sounds\\player\\cooldown1.wav"); 
    pSoundEffectsPlayer->AddSound("Sounds\\player\\cooldown2.wav"); // 14
	pSoundEffectsPlayer->AddSound("Sounds\\teleport\\teleport1.wav");
	pSoundEffectsPlayer->AddSound("Sounds\\teleport\\teleport2.wav"); // 16
    pSoundEffectsPlayer->AddSound("Sounds\\walk\\footstep1.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\walk\\footstep2.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\walk\\footstep3.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\walk\\footstep4.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\walk\\footstep5.wav");
    pSoundEffectsPlayer->AddSound("Sounds\\walk\\footstep6.wav"); //22

    pFreeViewCamera->SetLocalPosition({ 4.0f, 11.0f, -28.0f });
    pPlayer->SetLocalPosition({ 0.0f, 80.0f, -32.0f });

    pSceneRoot->AddComponent(
        std::make_unique<Global>(pSceneRoot.get(), wnd, pPlayer, pBase)
    );

    pLeftHand->AddComponent(
        std::make_unique<ModelComponent>(pLeftHand, wnd.Gfx(), "Models\\hands\\left.gltf", 1.0f, true, false)
    );
    pLeftHand->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pLeftHand->SetLocalScale({ 0.035f, 0.035f, 0.035f });
    pLeftHand->SetLocalPosition({ -0.0f, -1.0f, 0.5f });

    pLeftHand->AddComponent(
        std::make_unique<AnimationComponent>(pLeftHand, "", "Models\\hands\\left.gltf")
    );
    AnimationComponent* animCompLeft = pLeftHand->GetComponent<AnimationComponent>();
    animCompLeft->PlayAnimation(13); //POCZĄTKOWE IDLE_RUN


    pRightHand->AddComponent(
        std::make_unique<ModelComponent>(pRightHand, wnd.Gfx(), "Models\\hands\\right.gltf", 1.0f, true, false)
    );
    pRightHand->GetComponent<ModelComponent>()->LinkTechniques(rg);
    pRightHand->SetLocalScale({ 0.035f, 0.035f, 0.035f });
    pRightHand->SetLocalPosition({ -0.0f, -1.0f, 0.5f });

    pRightHand->AddComponent(
        std::make_unique<AnimationComponent>(pRightHand, "", "Models\\hands\\right.gltf")
    );
    AnimationComponent* animCompRight = pRightHand->GetComponent<AnimationComponent>();
    animCompRight->PlayAnimation(13); //POCZĄTKOWE IDLE_RUN


    pAbility1->GetComponent<Ability1>()->leftHand = pLeftHand->GetComponent<AnimationComponent>();

    pAbility2->GetComponent<Ability2>()->rightHand = pRightHand->GetComponent<AnimationComponent>();

    pAbility3->GetComponent<Ability3>()->leftHand = pLeftHand->GetComponent<AnimationComponent>();
    pAbility3->GetComponent<Ability3>()->rightHand = pRightHand->GetComponent<AnimationComponent>();

    pAbility4->GetComponent<Ability4>()->leftHand = pLeftHand->GetComponent<AnimationComponent>();

	pAbility5->GetComponent<Ability5>()->rightHand = pRightHand->GetComponent<AnimationComponent>();

    pAbility6->GetComponent<Ability6>()->leftHand = pLeftHand->GetComponent<AnimationComponent>();

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

	//PrefabManager::InstantiateIslandSmall1(pSceneRoot.get(), Vector3(60.0f, 50.0f, 0.0f), 1.0f);
    //PrefabManager::InstantiateMiniBossIsland(pSceneRoot.get(), Vector3(-400.0f, 50.0f, 0.0f), 1.0f);
    tutorialNode->AddComponent(
        std::make_unique<Tutorial>(tutorialNode, wnd, pPlayer)
    );

        
        
    //pSceneRoot->GetComponent<Global>()->tut = tutorialNode->GetComponent<Tutorial>();


    const int screenWidth = wnd.GetWidth();
    const int screenHeight = wnd.GetHeight();
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
        L"Images\\zawijas.png"
    );

    hurtSprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        0,
        0,
        wnd.Gfx().GetWidth(),
        wnd.Gfx().GetHeight(),
        L"Images\\health\\hurt.png"
    );


    heart0Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        wnd.Gfx().GetWidth() / 2 - (wnd.Gfx().GetWidth() * 0.35f) / 2,
        wnd.Gfx().GetHeight() * 0.93f,
        wnd.Gfx().GetWidth() * 0.35f,
        wnd.Gfx().GetHeight() * 0.05f,
        L"Images\\health\\hp_0.png"
    );

    heart1Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        wnd.Gfx().GetWidth() / 2 - (wnd.Gfx().GetWidth() * 0.35f) / 2,
        wnd.Gfx().GetHeight() * 0.93f,
        wnd.Gfx().GetWidth() * 0.35f,
        wnd.Gfx().GetHeight() * 0.05f,
        L"Images\\health\\hp_1.png"
    );

    heart2Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        wnd.Gfx().GetWidth() / 2 - (wnd.Gfx().GetWidth() * 0.35f) / 2,
        wnd.Gfx().GetHeight() * 0.93f,
        wnd.Gfx().GetWidth() * 0.35f,
        wnd.Gfx().GetHeight() * 0.05f,
        L"Images\\health\\hp_2.png"
    );

    heart3Sprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),    
        wnd.Gfx().GetContext(),
        wnd.Gfx().GetWidth() / 2 - (wnd.Gfx().GetWidth() * 0.35f) / 2,
        wnd.Gfx().GetHeight() * 0.93f,
        wnd.Gfx().GetWidth() * 0.35f,
        wnd.Gfx().GetHeight() * 0.05f,
        L"Images\\health\\hp_3.png"
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
    blackSprite = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(),
        wnd.Gfx().GetContext(),
        0,
        0,
        wnd.Gfx().GetWidth(),
        wnd.Gfx().GetHeight(),
        L"Images\\black.png" // Make sure you have this file
    );

    cursorEnabled = true;
    wnd.EnableCursor();  
    wnd.mouse.DisableRawInput();
    
    
    LoadMainMenuResources();

    //resumeBttn = std::make_unique<Sprite>(
    //    wnd.Gfx().GetDevice(),
    //    wnd.Gfx().GetContext(),
    //    (wnd.GetWidth() * 0.354f), (wnd.GetHeight() * 0.339f), (wnd.GetWidth() * 0.296f), (wnd.GetHeight() * 0.110f),
    //    L"Images\\resume.png"
    //); 
    //backBttn = std::make_unique<Sprite>(
    //    wnd.Gfx().GetDevice(),
    //    wnd.Gfx().GetContext(),
    //    (wnd.GetWidth() * 0.354f), (wnd.GetHeight() * 0.489f), (wnd.GetWidth() * 0.296f), (wnd.GetHeight() * 0.110f),
    //    L"Images\\back_to_menu.png"
    //); 
    //pauseMenuBackground = std::make_unique<Sprite>(
    //    wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
    //    0, 0, wnd.GetWidth(), wnd.GetHeight(),
    //    L"Images\\pause_menu.png"
    //); 
    //startBttn = std::make_unique<Sprite>(
    //    wnd.Gfx().GetDevice(),
    //    wnd.Gfx().GetContext(),
    //    (wnd.GetWidth() * 0.055f), (wnd.GetHeight() * 0.543f), (wnd.GetWidth() * 0.229f), (wnd.GetHeight() * 0.098f),
    //    L"Images\\start_2.png"
    //); 
    //quitBttn = std::make_unique<Sprite>(
    //    wnd.Gfx().GetDevice(),
    //    wnd.Gfx().GetContext(),
    //    (wnd.GetWidth() * 0.055f), (wnd.GetHeight() * 0.673f), (wnd.GetWidth() * 0.229f), (wnd.GetHeight() * 0.098f),
    //    L"Images\\quit_2.png"
    //);
    // 
    //mainMenuBackground = std::make_unique<Sprite>(
    //    wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
    //    0, 0, wnd.GetWidth(), wnd.GetHeight(),
    //    L"Images\\menu_compressed_6.gif"
    //);
 
  
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
        if (!paused || gameReset > 0)
        {
            while (lag >= FIXED_TIME_STEP)
            {
                physicsSystem->Update(FIXED_TIME_STEP, 1, temp_allocator, job_system);
                lag -= FIXED_TIME_STEP;
            }
        }
        else
        {
            while (lag >= FIXED_TIME_STEP)
                lag -= FIXED_TIME_STEP;
        }
        const float alpha = lag / FIXED_TIME_STEP;

        HandleInput(dt);
        DoFrame(dt);

    }
}

void App::HandleInput(float dt)
{
    if (wnd.kbd.IsJustPressed('B'))
    {
        PrefabManager::InstantiateBossEnemy(temporary, pFreeViewCamera->GetWorldPosition());
        //PrefabManager::InstantiateMushroom1(temporary, pFreeViewCamera->GetWorldPosition(), 1.0f);

    }
    if (wnd.kbd.IsJustPressed('M'))
    {
        if (myMusic->isPlaying())
        {
            myMusic->Stop();
        }
        else
        {
            myMusic->Play();
        } 
    }

    if (wnd.kbd.IsJustPressed('C'))
    {
        if (cursorEnabled) {
            wnd.DisableCursor();
            wnd.mouse.EnableRawInput();
        }
        else {
            wnd.EnableCursor();
            wnd.mouse.DisableRawInput();
        }
        cursorEnabled = !cursorEnabled;
    }

    if (wnd.kbd.IsJustPressed('H'))
    {
        showControlWindow = !showControlWindow;
    }
    if (wnd.kbd.IsJustPressed('N'))
    {
        tutorialNode->GetComponent<Tutorial>()->completed = true;
        pPlayer->SetWorldPosition({ 0.0f, 20.0f, 10.0f });
    }
    if (wnd.kbd.IsJustPressed('Z'))
    {
        StartGame();
    }
    if (wnd.kbd.IsJustPressed('X'))
    {
        ResetGame();
    }
    if (wnd.kbd.IsJustPressed('P'))
    {
        paused = !paused;
    }
    if (wnd.kbd.IsJustPressed(VK_ESCAPE) && startedGame)
    {
        paused = !paused;

        if (paused) {
            LoadPauseMenuResources();
            cursorEnabled = true;
            wnd.EnableCursor();
            wnd.mouse.DisableRawInput();
        } else 
        {
            UnloadPauseMenuResources();
            cursorEnabled = false;
            wnd.DisableCursor();
            wnd.mouse.EnableRawInput();
        }
        //PostQuitMessage(0);
        //return; // Zwróć od razu, aby uniknąć dalszego przetwarzania
    }

    if (wnd.kbd.IsJustPressed(VK_F1))
    {
        showDemoWindow = !showDemoWindow;
    }

    if (wnd.kbd.IsJustPressed('V'))
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
    wnd.Gfx().BeginFrame(0.5f, 0.5f, 1.0f);
    //fizyka
    if (!paused || gameReset > 0)
    {
        gameReset--;
        auto* contact = dynamic_cast<MyContactListener*>(physicsSystem->GetContactListener());
        contact->ExecuteTriggerActivationQueue();
        contact->ExecuteCollisionActivationQueue();
        CleanupDestroyedNodes(pSceneRoot.get());    //data removed
        pSceneRoot->Update(dt);
        RemoveRigidbody(pSceneRoot.get());//rigidbody remove if destruction
        JPH::BodyIDVector bodyIDs;
        PhysicsCommon::physicsSystem->GetBodies(bodyIDs);
        for (JPH::BodyID bodyID : bodyIDs)
        {
            JPH::uint64 data = PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(bodyID);
            if (data == 0)
            {
                PhysicsCommon::physicsSystem->GetBodyInterface().DeactivateBody(bodyID);
                PhysicsCommon::physicsSystem->GetBodyInterface().RemoveBody(bodyID);
                PhysicsCommon::physicsSystem->GetBodyInterface().DestroyBody(bodyID);
            }
        }
    }
    

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
    Bind::TransformCbuf::SetLight(&dirLight);
    Bind::ShadowCbuf::SetLight(&dirLight);
    DirectX::XMFLOAT3 focusPos = pPlayer->GetWorldPosition();
    Camera* activePlayerCamera = pCamera->GetComponent<Camera>();
    /* if (freeViewCamera) {
         activePlayerCamera = pFreeViewCamera->GetComponent<Camera>();
     }
     else {
         activePlayerCamera = pCamera->GetComponent<Camera>();
     }*/
    dirLight.Update(wnd.Gfx(), focusPos, *activePlayerCamera);

    // --- Bind Lights ---
    // pointLight.Bind(wnd.Gfx(), viewMatrix);
    dirLight.Bind(wnd.Gfx());

    FrustumCalculating();

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
    if (targetSprite) {
        targetSprite->Draw(wnd.Gfx().GetContext());
    }

    //hardcode hurt info

    if (pPlayer != nullptr && pPlayer->GetComponent<Health>() != nullptr)
    {


        static float previousHealth = 3.0f;
        const float hurtEffectDuration = 1.0f;
        static float hurtEffectTimer = hurtEffectDuration;

        if (pSceneRoot->GetComponent<Global>()->drawLoadingScreen) {
            hurtEffectTimer = hurtEffectDuration; // Ustaw timer na "zakończony"
        }

        float currentHealth = pPlayer->GetComponent<Health>()->currentHealth;

        if (currentHealth < previousHealth) {
            hurtEffectTimer = 0.0f;
        }
        previousHealth = currentHealth;

        if (hurtEffectTimer < hurtEffectDuration) {
            hurtEffectTimer += dt;
        }

        if (hurtEffectTimer < hurtEffectDuration) {
            float t = hurtEffectTimer / hurtEffectDuration;
             
            float t_eased_scale = 1.0f - (1.0f - t) * (1.0f - t);
            float scale = 1.5f - 0.5f * t_eased_scale;
             
            float alpha = 1.0f; 
            float fadeOutStartTime = 0.5f;
            if (t > fadeOutStartTime) { 
                float fade_t = (t - fadeOutStartTime) / (1.0f - fadeOutStartTime);
                alpha = 1.0f - fade_t;
            }
             
            DirectX::XMFLOAT4 hurtColor = { 1.0f, 1.0f, 1.0f, alpha };
             
            hurtSprite->Draw(wnd.Gfx().GetContext(), std::max(1.0f, scale), hurtColor);
        }

        // --- Health bar drawing logic ---
        if (currentHealth == 3.0f) {
            heart3Sprite->Draw(wnd.Gfx().GetContext());
        }
        else if (currentHealth == 2.0f) {
            heart2Sprite->Draw(wnd.Gfx().GetContext());
        }
        else if (currentHealth == 1.0f) {
            heart1Sprite->Draw(wnd.Gfx().GetContext());
        }
        else if (currentHealth <= 0.0f) {
            heart0Sprite->Draw(wnd.Gfx().GetContext());
        }
    }

    pUpgradeHandler->DrawUpgradeMenu();

     
    if (pSceneRoot->GetComponent<Global>()->drawLoadingScreen || bonusTime > 0.0f)
    { 
        static bool wasActive = false;
        static float fadeOutTimer = 0.0f;
        const float fadeOutDuration = 2.5f; // Czas trwania samego fade out
         
        bool isActiveNow = pSceneRoot->GetComponent<Global>()->drawLoadingScreen;
         
        if (isActiveNow) {
            bonusTime = fadeOutDuration;  
        }
        else if (bonusTime > 0.0f) {
            bonusTime -= dt;
        }
         
        float alpha = 0.0f;
         
        if (isActiveNow && !wasActive) {
            fadeOutTimer = 0.0f;
        }
        wasActive = isActiveNow;

        if (isActiveNow) { 
            alpha = 1.0f;
        }
        else { 
            if (fadeOutTimer < fadeOutDuration) {
                fadeOutTimer += dt;
                alpha = 1.0f - (fadeOutTimer / fadeOutDuration);
            }
        }
         
        alpha = std::max(0.0f, std::min(1.0f, alpha));
        if (alpha > 0.0f) {
            blackSprite->Draw(wnd.Gfx().GetContext(), 1.0f, { 1.0f, 1.0f, 1.0f, alpha });
        }
    }
    if (paused && startedGame)
    {
        DrawPauseMenu(dt);
    }
    if (paused && !startedGame)
    {
        DrawMainMenu(dt);

    }


    wnd.kbd.UpdateFrameState();

    wnd.Gfx().EndFrame();
    rg.Reset();

    if (!pSceneRoot->GetComponent<Global>()->drawLoadingScreen) {
        if (myMusic->musicStart) {
            myMusic->Play();
            myMusic->musicStart = false;
        }
        if (myMusic->isPlaying())
        {
            myMusic->UpdateBufferStream();
        }
    }
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
    const float nearDist = 0.3f;        
    const float farDist = 2000.0f;      

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

    cameraFrustum.Near = 0.3f;

    cameraFrustum.Far = 2000.0f;

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
            if(node->GetComponent<SpawnAttack>() != nullptr)
            {
                node->GetComponent<SpawnAttack>()->undercover = true;
			}
        }

    }

    if (shouldDraw)
    {
        node->Submit(wnd.Gfx());
        if (node->GetComponent<SpawnAttack>() != nullptr)
        {
            node->GetComponent<SpawnAttack>()->undercover = false;
        }
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

void App::StartGame()
{
    if (startedGame || !paused) return;
    startedGame = true;
    paused = false;
    UnloadMainMenuResources();

    PrefabManager::InstantiateTutorialIslands(tutorialNode, tutorialNode->GetComponent<Tutorial>() , Vector3(0.0f, 0.0f, 0.0f), 1.0f);
    tutorialNode->GetComponent<Tutorial>()->Start();
    pSceneRoot->GetComponent<Global>()->Start();
    
}

void App::ResetGame()
{
    if (!startedGame || !paused) return;

    UnloadPauseMenuResources();
    LoadMainMenuResources();


    startedGame = false;
    gameReset = 2;
    pSceneRoot->GetComponent<Global>()->Reset();
    tutorialNode->GetComponent<Tutorial>()->Reset();
    pSelectedSceneNode = nullptr;
    if (temporary != nullptr)
    {
        temporary->DestroyChilds();
    }
    if (tutorialNode != nullptr)
    {
        tutorialNode->DestroyChilds();
    }
}


void App::SaveNodeTransformsRecursive(Node& node, std::ofstream& file)
{
    if (node.GetName() != "Root" && node.GetName() != "L Normal" && node.GetName() != "Camera" && node.GetName() != "FreeViewCamera" &&
        node.GetName() != "L Ability" && node.GetName() != "R Normal" && node.GetName() != "R Ability" && node.GetName() != "Player")
    {
        DirectX::XMFLOAT3 pos = node.GetLocalPosition();
        DirectX::XMFLOAT3 rot = node.GetLocalRotationEuler();

        file << std::fixed << std::setprecision(2);

        file << "Object: " << std::setw(20) << std::left << node.GetName()
            << " Position: {" << pos.x << "f, " << pos.y << "f, " << pos.z << "f}" 
			<< " Rotation: {" << rot.x << "f, " << rot.y << "f, " << rot.z << "f}"
            << std::endl;
    }

    for (const auto& pChild : node.GetChildren())
    {
        if (pChild)
        {
            SaveNodeTransformsRecursive(*pChild, file);
        }
    }
}
 
 
 
 


void App::DrawPauseMenu(float dt)
{

    //mainMenuBackground->Update(dt);
    //mainMenuBackground->Draw(wnd.Gfx().GetContext());
    pauseMenuBackground->Draw(wnd.Gfx().GetContext());
    int mouseX = wnd.mouse.GetPosX();
    int mouseY = wnd.mouse.GetPosY();
    bool isMouseLeftPressedThisFrame = wnd.mouse.IsLeftPressed();

 
    if (mouseX >= (wnd.GetWidth() * 0.354f) && mouseX <= ((wnd.GetWidth() * 0.354f) + (wnd.GetWidth() * 0.296f)) &&
        mouseY >= (wnd.GetHeight() * 0.339f) && mouseY <= ((wnd.GetHeight() * 0.339f) + (wnd.GetHeight() * 0.110f))) {
        resumeBttn->Draw(wnd.Gfx().GetContext());
        if (isMouseLeftPressedThisFrame) {
            paused = false;
            wnd.DisableCursor();
            wnd.mouse.EnableRawInput();
            cursorEnabled = false;
        }
    }

    if (mouseX >= (wnd.GetWidth() * 0.354f) && mouseX <= ((wnd.GetWidth() * 0.354f) + (wnd.GetWidth() * 0.296f)) &&
        mouseY >= (wnd.GetHeight() * 0.489f) && mouseY <= ((wnd.GetHeight() * 0.489f) + (wnd.GetHeight() * 0.110f))) {
        backBttn->Draw(wnd.Gfx().GetContext());
        if (isMouseLeftPressedThisFrame) {
            ResetGame();  

        }
    } 

 
}


void App::DrawMainMenu(float dt)
{

    int mouseX = wnd.mouse.GetPosX();
    int mouseY = wnd.mouse.GetPosY();
    bool isMouseLeftPressedThisFrame = wnd.mouse.IsLeftPressed();
    mainMenuBackground->Update(dt);
    mainMenuBackground->Draw(wnd.Gfx().GetContext());
 
    if (mouseX >= (wnd.GetWidth() * 0.055f) && mouseX <= ((wnd.GetWidth() * 0.055f) + (wnd.GetWidth() * 0.229f)) &&
        mouseY >= (wnd.GetHeight() * 0.543f) && mouseY <= ((wnd.GetHeight() * 0.543f) + (wnd.GetHeight() * 0.098f))) {
        startBttn->Draw(wnd.Gfx().GetContext());
        if (isMouseLeftPressedThisFrame) {
            paused = true;
            wnd.DisableCursor();
            wnd.mouse.EnableRawInput();
            cursorEnabled = false;
            StartGame();
        }
    }

    if (mouseX >= (wnd.GetWidth() * 0.055f) && mouseX <= ((wnd.GetWidth() * 0.055f) + (wnd.GetWidth() * 0.229f)) &&
        mouseY >= (wnd.GetHeight() * 0.673f) && mouseY <= ((wnd.GetHeight() * 0.673f) + (wnd.GetHeight() * 0.098f))) {
        quitBttn->Draw(wnd.Gfx().GetContext());
        if (isMouseLeftPressedThisFrame) {
            PostQuitMessage(0);
        }
    } 
 }

void App::LoadMainMenuResources()
{
    // Jeśli zasoby już istnieją, nie rób nic
    if (mainMenuBackground) return;

    OutputDebugStringA("Loading Main Menu Resources...\n");
    const int screenWidth = wnd.GetWidth();
    const int screenHeight = wnd.GetHeight();

    mainMenuBackground = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
        0, 0, screenWidth, screenHeight,
        L"Images\\menu_compressed_6.gif"
    );
    startBttn = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
        (screenWidth * 0.055f), (screenHeight * 0.543f), (screenWidth * 0.229f), (screenHeight * 0.098f),
        L"Images\\start_2.png"
    );
    quitBttn = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
        (screenWidth * 0.055f), (screenHeight * 0.673f), (screenWidth * 0.229f), (screenHeight * 0.098f),
        L"Images\\quit_2.png"
    );
}

void App::UnloadMainMenuResources()
{
    // Jeśli zasoby nie istnieją, nie rób nic
    if (!mainMenuBackground) return;

    OutputDebugStringA("Unloading Main Menu Resources...\n");
    mainMenuBackground.reset();
    startBttn.reset();
    quitBttn.reset();
}

void App::LoadPauseMenuResources()
{
    if (pauseMenuBackground) return;

    OutputDebugStringA("Loading Pause Menu Resources...\n");
    const int screenWidth = wnd.GetWidth();
    const int screenHeight = wnd.GetHeight();

    pauseMenuBackground = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
        0, 0, screenWidth, screenHeight,
        L"Images\\pause_menu.png"
    );
    resumeBttn = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
        (screenWidth * 0.354f), (screenHeight * 0.339f), (screenWidth * 0.296f), (screenHeight * 0.110f),
        L"Images\\resume.png"
    );
    backBttn = std::make_unique<Sprite>(
        wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
        (screenWidth * 0.354f), (screenHeight * 0.489f), (screenWidth * 0.296f), (screenHeight * 0.110f),
        L"Images\\back_to_menu.png"
    );
}

void App::UnloadPauseMenuResources()
{
    if (!pauseMenuBackground) return;

    OutputDebugStringA("Unloading Pause Menu Resources...\n");
    pauseMenuBackground.reset();
    resumeBttn.reset();
    backBttn.reset();
}