#include "SpawnJump.h"
#include "Global.h"
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
			//if((playerNode->GetWorldPosition() - Vector3(1.0f, 1.0f, 1.0f).Length())
			jumpSoundTimer -= dt;
			if (!upgraded) {
				if (playerNode->GetComponent<SoundEffectsPlayer>())
				{
					if (jumpSoundTimer <= 0.0f)
					{
						pOwner->GetComponent<SoundEffectsPlayer>()->Play(0, 0.7f);

						jumpSoundTimer = jumpSoundInterval;
					}
				}
			}
			if (!upgraded && !playerReady && (pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 7.0f && jumpCooldown < 0.1)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
				pOwner->GetRoot()->GetComponent<Global>()->NextStage();
				playerReady = true;
			}
			if (upgraded)
			{	
				//pOwner->GetComponent<SoundEffectsPlayer>()->Stop(0);
				if ((pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 7.0f && jumpCooldown < 0.1)
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
		}
	}
	if (jumpCooldown > 0.0f) jumpCooldown -= dt;
}

void SpawnJump::Activate()
{
	pOwner->GetComponent<SoundEffectsPlayer>()->Play(1, 1.5f, false);
	activated = true;
}

void SpawnJump::DrawImGuiControls()
{

}