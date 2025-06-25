#include "SpawnJump.h"
#include "Global.h"
#include "PrefabManager.h"
namespace dx = DirectX;
SpawnJump::SpawnJump(Node* owner, Window& window, Node* player)
	: Component(owner), wnd(window), playerNode(player)
{
}


void SpawnJump::Update(float dt)
{
	if (activated)
	{
		if (playerNode != nullptr)
		{
			Vector3 dir = playerNode->GetWorldPosition() - pOwner->GetWorldPosition();
			dir.x = 0.0f;
			dir.z = 0.0f;
			
			jumpSoundTimer -= dt;

			if (!upgraded && !playerReady && (pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 10.0f && jumpCooldown < 0.1)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
				pOwner->GetRoot()->GetComponent<Global>()->NextStage();
				playerReady = true;
			}
			if (upgraded)
			{	
				if ((pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 10.0f && jumpCooldown < 0.1)
				{
					if (playerNode->GetComponent<SoundEffectsPlayer>())
					{
						float randSound = (rand() % 2 + 14);
						playerNode->GetComponent<SoundEffectsPlayer>()->Play(randSound, 1.0f, false);
					}
					PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 280.0f, 0.0f));
					jumpCooldown = 0.5f;
				}
			}
			if (playerNode->GetComponent<SoundEffectsPlayer>())
			{
				if (jumpSoundTimer <= 0.0f)
				{
					if (dir.Length() < 200.0f) {
						pOwner->GetComponent<SoundEffectsPlayer>()->PlayAdvanced(0, 1.0f, false, 0.4f, 50.0f, 1000.0f, 1.0f, false);
					}
					else
					{
						pOwner->GetComponent<SoundEffectsPlayer>()->PlayAdvanced(0, 0.1f, false, 5.0f, 1.0f, 5.0f, 1.0f, false);
					}

					jumpSoundTimer = jumpSoundInterval;
				}
			}
		}
	}
	if (jumpCooldown > 0.0f) jumpCooldown -= dt;
}

void SpawnJump::Activate()
{
	pOwner->GetComponent<SoundEffectsPlayer>()->PlayAdvanced(1, 2.0f, false, 0.2f, 30.0f, 1000.0f, 1.0, false);
	jumpSoundTimer = jumpSoundInterval - 0.5f;
	activated = true;
	PrefabManager::InstantiateJumpPadParticles(pOwner, Vector3(0, 0, 0), 1.0f);
	PrefabManager::InstantiateJumpPadSmokeParticles(pOwner, Vector3(0, 0, 0), 1.0f);
	PrefabManager::InstantiateJumpPadActivationParticles(pOwner, Vector3(0, 0, 0), 1.0f);
}

void SpawnJump::DrawImGuiControls()
{

}