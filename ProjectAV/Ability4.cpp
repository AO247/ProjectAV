////////////////////////////////
// Evolve of ability 1        //
// Pick item then pick a spot //
////////////////////////////////
#include "Ability4.h"
#include "Node.h"       
#include "Window.h"     
#include "CMath.h"      
#include <DirectXMath.h>
#include <algorithm> 
#include <string>
#include "PrefabManager.h"

namespace dx = DirectX;
Ability4::Ability4(Node* owner, Window& window, Node* camera)
    : Ability(owner, window, camera)
{
    player = pOwner->GetRoot()->FindFirstChildByTag("PLAYER");
}


void Ability4::Update(float dt)
{
    if (!wnd.CursorEnabled())
    {
        pOwner->SetWorldPosition(player->GetWorldPosition());
        holdSoundTimer -= dt;
        if (isPressed)
        {
			pressedTime += dt;
            if (pressedTime > 0.13f && activated)
            {
                activated = false;
                // animacja wyboru obiektu
                // dŸwiêk wyboru obiektu
                if (pOwner->GetComponent<SoundEffectsPlayer>()) {
                    float randSound = (rand() % 2);
                    pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
                }
                
                // particle wyboru obiektu
                //  
                leftHand->PlayAnimation(4); //ATTACK_PULL_LOOP

            }
            else if (pressedTime > 0.13f)
            {

                leftHand->PlayAnimation(4); //ATTACK_PULL_LOOP
                // animacja trzymania 
                // dŸwiêk trzymania 
                if (pOwner->GetComponent<SoundEffectsPlayer>()) {
                    if (holdSoundTimer <= 0.0f)
                    {
                        pOwner->GetComponent<SoundEffectsPlayer>()->Play(2, 1.0f, false);

                        holdSoundTimer = holdSoundInterval;
                    }
                }
            }
        }
        Positioning();
        Cooldowns(dt);
    }
}
void Ability4::Positioning()
{
    if (isPressed) return;
    Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
    Vec3 direction = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
    RRayCast ray = RRayCast(position, direction * 100.0f);
    RayCastResult result;
    if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
        MultipleBroadPhaseLayerFilter({ BroadPhaseLayers::WALL }),
        MultipleObjectLayerFilter({ Layers::WALL })))
    {
        position = ray.mOrigin + ray.mDirection * result.mFraction;
        if (PhysicsCommon::physicsSystem->GetBodyInterface().GetMotionType(result.mBodyID) == EMotionType::Dynamic)
        {
            selectedNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(result.mBodyID));
            if (selectedNode->GetComponent<Throwable>()->extraHeavy)
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
bool Ability4::Pressed()
{
    if (!abilityReady && isPressed) return false;

    activated = true;
    isPressed = true;
    pressedTime = 0.0f;

    cameraRotation = camera->GetLocalRotationEuler();
    if (selectedNode != nullptr)
    {
        selectionParticles = PrefabManager::InstantiateAbility4SelectParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0);
        selectionParticlesSmoke = PrefabManager::InstantiateAbility4SelectSmokeParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0);
    }

    return true;
}
void Ability4::Released()
{
    if (!isPressed) return;
    isPressed = false;
    if (!abilityReady) return;


 
    timeToChange = 0.3f;

    if (pressedTime < 0.13f || selectedNode == nullptr)
    {
        pressedTime = 0.0f;
		pOwner->GetComponent<SoundEffectsPlayer>()->Stop(2);
        baseAbility->Pressed();
    }
    else
    {
        // particle rzutu
        // animacja rzutu
        // dŸwiêk rzutu
        if (baseAbility->GetOwner()->GetComponent<SoundEffectsPlayer>()) {
            pOwner->GetComponent<SoundEffectsPlayer>()->Stop(2);
            float randSound = rand() % 4;
            baseAbility->GetOwner()->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f);
        }
        

        leftHand->PlayAnimation(3, 0.2f, false);
        if (selectionParticles != nullptr)
        {
            selectionParticles->GetComponent<ParticleSystemComponent>()->Stop();
            selectionParticles = nullptr;
        }

        if (selectionParticlesSmoke != nullptr)
        {
            selectionParticlesSmoke->GetComponent<ParticleSystemComponent>()->Stop();
            selectionParticlesSmoke = nullptr;
        }
        pressedTime = 0.0f;
        Vector3 direction = Vector3::Zero;
        Vec3 position = Vec3(camera->GetWorldPosition().x, camera->GetWorldPosition().y, camera->GetWorldPosition().z);
        Vec3 dire = Vec3(camera->Forward().x, camera->Forward().y, camera->Forward().z);
        RRayCast ray = RRayCast(position, dire * 100.0f);
        RayCastResult result;
        if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
            IgnoreMultipleBroadPhaseLayerFilter({ BroadPhaseLayers::PLAYER, BroadPhaseLayers::TRIGGER }),
            IgnoreMultipleObjectLayerFilter({ Layers::PLAYER, Layers::TRIGGER })))
        {
            position = ray.mOrigin + ray.mDirection * result.mFraction;
            Vector3 pos = Vector3(position.GetX(), position.GetY(), position.GetZ());
            direction = pos - selectedNode->GetWorldPosition();
            direction.Normalize();
        }
        else
        {
            Vector3 dir = camera->GetWorldPosition() - selectedNode->GetWorldPosition();

            float yaw = std::atan2(dir.x, dir.z);
            float distanceXZ = std::sqrt(dir.x * dir.x + dir.z * dir.z);
            float pitch = -std::atan2(dir.y, distanceXZ); // Negative for DirectX

            pOwner->SetLocalRotation(DirectX::XMFLOAT3(pitch, yaw, 0.0f));

            Vector3 newCameraRot = camera->GetLocalRotationEuler();

            float deltaYaw = wrap_angle(newCameraRot.y - cameraRotation.y);
            float deltaPitch = wrap_angle(newCameraRot.x - cameraRotation.x);


            direction = pOwner->Right() * -deltaYaw + pOwner->Up() * -deltaPitch;

            if (direction.Length() < 0.01f)
                direction = pOwner->Back();

            direction.Normalize();
        }

        Vector3 target = Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z) + direction;

        DirectX::XMVECTOR viewerPosition = DirectX::XMVectorSet(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z, 0.0f);
        DirectX::XMVECTOR targetPosition = DirectX::XMVectorSet(target.x, target.y, target.z, 0.0f);
        DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

        DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixLookAtLH(viewerPosition, targetPosition, up);

        DirectX::XMMATRIX objectRotationMatrix = DirectX::XMMatrixInverse(nullptr, lookAtMatrix);

        DirectX::XMVECTOR lookAtQuaternion = DirectX::XMQuaternionRotationMatrix(objectRotationMatrix);
        DirectX::XMFLOAT4 quatFloat4;
        DirectX::XMStoreFloat4(&quatFloat4, lookAtQuaternion);

        PrefabManager::InstantiateAbility4ReleaseParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0, quatFloat4);
        PrefabManager::InstantiateAbility4ReleaseSmokeParticles(pOwner->GetParent(), Vector3(selectedNode->GetWorldPosition().x, selectedNode->GetWorldPosition().y, selectedNode->GetWorldPosition().z), 1.0, quatFloat4);

        PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(selectedNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
        PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(selectedNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(direction.x, direction.y, direction.z) * force);

    }
    cooldownTimer = cooldown;
    abilityReady = false;
}


void Ability4::Cooldowns(float dt)
{
    if (cooldownTimer > 0.0f)
    {
        cooldownTimer -= dt;
        if (leftHand->GetCurrentPlayingAnimationRaw() == nullptr) {
            leftHand->PlayAnimation(8); //COOLDOWN
        }
    }
    else
    {
        if (!abilityReady)
        {
            leftHand->PlayAnimation(13);  //IDLE RUN
        }
        abilityReady = true;
    }
    if (timeToChange > 0.0f)
    {
        timeToChange -= dt;
        if (timeToChange <= 0.0f)
        {
            //leftHandAbility->SetLocalPosition({ 0.0f, -2.7f, 3000.0f });
            //leftHand->SetLocalPosition({ 0.0f, -2.7f, 1.0f });
        }
    }

}

void Ability4::DrawImGuiControls()
{
    ImGui::InputFloat("Force", &force);
    ImGui::InputFloat("Cooldown", &cooldown);

}