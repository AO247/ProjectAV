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
	{//player y - this y.length() < 100.0f
		if (playerNode != nullptr)
		{
			jumpSoundTimer -= dt;
			if (!upgraded) {
				if (playerNode->GetComponent<SoundEffectsPlayer>())
				{
					if (jumpSoundTimer <= 0.0f)
					{
						playerNode->GetComponent<SoundEffectsPlayer>()->Play(17);

						jumpSoundTimer = jumpSoundInterval;
					}
				}
			}
			if (!upgraded && !playerReady && (pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 10.0f && jumpCooldown < 0.1)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
				pOwner->GetRoot()->GetComponent<Global>()->NextStage();
				playerReady = true;
			}
			if (upgraded)
			{	
				playerNode->GetComponent<SoundEffectsPlayer>()->Stop(17);
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
		}
	}
	if (jumpCooldown > 0.0f) jumpCooldown -= dt;
}

void SpawnJump::Activate()
{
	playerNode->GetComponent<SoundEffectsPlayer>()->Play(16, 1.0f, false);
	activated = true;
}

void SpawnJump::DrawImGuiControls()
{

}