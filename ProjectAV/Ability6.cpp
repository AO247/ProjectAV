////////////////////////////////
// Evolve of ability 1        //
// Pick item then pick a spot //
////////////////////////////////
#include "Ability6.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm> 
#include <string>
#include "PrefabManager.h"

namespace dx = DirectX;
Ability6::Ability6(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{
    player = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
}


void Ability6::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        pOwner->SetWorldPosition(player->GetWorldPosition());
        PullingParticlesPositioning();
        Positioning();
        Cooldowns(dt);
    }
    if (isPressed)
    {
        Pulling(dt);
    }
}
void Ability6::Positioning()
{
    if (isPressed) return;
    Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
    Vec3 direction = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
    RRayCast ray = RRayCast(position, direction * 100.0f);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
        MultipleBroadPhaseLayerFilter({BroadPhaseLayers::WALL }),
        MultipleObjectLayerFilter({Layers::WALL })))
    {
        position = ray.mOrigin + ray.mDirection * result.mFraction;
        if (PhysicsCommon::physicsSystem->GetBodyInterface().GetMotionType(result.mBodyID) == EMotionType::Dynamic)
        {
            selectedNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(result.mBodyID));
            if (selectedNode->GetComponent<Throwable>() == nullptr)
            {
                selectedNode = nullptr;
            }

        }
    }
    else
    {
        selectedNode = nullptr;
    }
}
void Ability6::Pulling(float dt)
{
	if (selectedNode == nullptr) return;

    Vector3 cameraPos = camera->GetWorldPosition();
    Vector3 targetPosition = cameraPos + camera->Forward() * 13.0f;
    Rigidbody* rb = selectedNode->GetComponent<Rigidbody>();

    holdSoundTimer -= dt;

    Vector3 tempDir = targetPosition - selectedNode->GetWorldPosition();
    tempDir.y = 0.0f;
    if (tempDir.Length() < 3.0f)
    {
        if (pullingParticles != nullptr)
        {
            pullingParticles->GetComponent<ParticleSystemComponent>()->Stop();
            pullingParticles = nullptr;
        }
        if (holdParticles == nullptr)
        {
            holdParticles = PrefabManager::InstantiateAbility6HoldParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0);
        }
        else
        {
            holdParticles->SetWorldPosition(Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z));
        }

        if (holdSmokeParticles == nullptr)
        {
            holdSmokeParticles = PrefabManager::InstantiateAbility6HoldSmokeParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0);
        }
        else
        {
            holdSmokeParticles->SetWorldPosition(Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z));
        }
        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rb->GetBodyID(), Vec3::sZero());
        PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(rb->GetBodyID(), Vec3(targetPosition.x, targetPosition.y, targetPosition.z), EActivation::Activate);

        // dzwiek trzymania obiektu
       // animacja trzymania obiektu
        leftHand->PlayAnimation(4); //PULL LOOP             TU CZY W ELSE

        holdSoundTimer -= dt;
        if (pOwner->GetComponent<SoundEffectsPlayer>()) {
            if (holdSoundTimer <= 0.0f)
            {
                pOwner->GetComponent<SoundEffectsPlayer>()->Play(2, 1.0f, false);

                holdSoundTimer = holdSoundInterval;
            }
        }
    }
    else
    {
       // leftHand->PlayAnimation(4); //PULL LOOP
        Vector3 dir = targetPosition - selectedNode->GetWorldPosition();
        dir.y *= 10.0f;
        dir.Normalize();
        PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(rb->GetBodyID(), Vec3(dir.x, dir.y, dir.z) * force * 100.0f * dt);
    }
}
void Ability6::PullingParticlesPositioning()
{
    if (pullingParticles != nullptr && selectedNode != nullptr)
    {
        Vector3 target = Vector3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);

        DirectX::XMVECTOR viewerPosition = DirectX::XMVectorSet(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z, 0.0f);
        DirectX::XMVECTOR targetPosition = DirectX::XMVectorSet(target.x, target.y, target.z, 0.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixLookAtLH(viewerPosition, targetPosition, up);

        DirectX::XMMATRIX objectRotationMatrix = DirectX::XMMatrixInverse(nullptr, lookAtMatrix);

        DirectX::XMVECTOR lookAtQuaternion = DirectX::XMQuaternionRotationMatrix(objectRotationMatrix);
        DirectX::XMFLOAT4 quatFloat4;
        DirectX::XMStoreFloat4(&quatFloat4, lookAtQuaternion);

        pullingParticles->SetWorldPosition(selectedNode->GetWorldPosition());
        pullingParticles->SetWorldRotation(quatFloat4);
    }
}
bool Ability6::Pressed()
{
    if (!abilityReady) return false;
    if (selectedNode == nullptr)
    {
        baseAbility->Pressed();
        cooldownTimer = cooldown;
        abilityReady = false;
        return true;
    }
    isPressed = true;
    // animacja przyciagniecia
    // particle dodanie do obiektu
    // dzwiek wyboru node
    if (pOwner->GetComponent<SoundEffectsPlayer>()) {
        float randSound = (rand() % 2);
        pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
    }
    //leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3.0f });
    //leftHand->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });

    leftHand->PlayAnimation(3, 0.2f, false); //ATTACK PULL
    Vector3 target = Vector3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);

    DirectX::XMVECTOR viewerPosition = DirectX::XMVectorSet(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z, 0.0f);
    DirectX::XMVECTOR targetPosition = DirectX::XMVectorSet(target.x, target.y, target.z, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixLookAtLH(viewerPosition, targetPosition, up);

    DirectX::XMMATRIX objectRotationMatrix = DirectX::XMMatrixInverse(nullptr, lookAtMatrix);

    DirectX::XMVECTOR lookAtQuaternion = DirectX::XMQuaternionRotationMatrix(objectRotationMatrix);
    DirectX::XMFLOAT4 quatFloat4;
    DirectX::XMStoreFloat4(&quatFloat4, lookAtQuaternion);

    if (pullingParticles == nullptr)
    {
        pullingParticles = PrefabManager::InstantiateAbility6PullingParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0, quatFloat4);
    }

    return true;
}
void Ability6::Released()
{
    if (!isPressed) return;
    isPressed = false;
    if (!abilityReady) return;
    // particle wylaczenie 
    if (pullingParticles != nullptr)
    {
        pullingParticles->GetComponent<ParticleSystemComponent>()->Stop();
        pullingParticles = nullptr;
    }
    if (holdParticles != nullptr)
    {
        holdParticles->GetComponent<ParticleSystemComponent>()->Stop();
        holdParticles = nullptr;
    }
    if (holdSmokeParticles != nullptr)
    {
        holdSmokeParticles->GetComponent<ParticleSystemComponent>()->Stop();
        holdSmokeParticles = nullptr;
    }
    //leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 4.0f });
    if (selectedNode->GetComponent<Throwable>()->extraHeavy == true)
    {
        abilityReady = false;
        //dzwiek faila 
        return;
    }
    timeToChange = 0.3f;
   
	pOwner->GetComponent<SoundEffectsPlayer>()->Stop(2);
    baseAbility->Pressed();

    cooldownTimer = cooldown;
    abilityReady = false;
}


void Ability6::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
        if (leftHand->GetCurrentPlayingAnimationRaw() == nullptr) {
            leftHand->PlayAnimation(8);
        }
    }
    else
    {
        if (!abilityReady)
        {
            leftHand->PlayAnimation(13);

        }
        abilityReady = true;
    }
  

}


void Ability6::DrawImGuiControls()
{
    ImGui::InputFloat("Force", &force);

    ImGui::InputFloat("Cooldown", &cooldown);

}