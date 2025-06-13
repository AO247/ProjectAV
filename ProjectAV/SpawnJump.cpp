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
			if (!upgraded && !playerReady && (pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 7.0f && jumpCooldown < 0.1)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
				pOwner->GetRoot()->GetComponent<Global>()->NextStage();
				playerReady = true;
			}
			if (upgraded)
			{
				if ((pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 7.0f && jumpCooldown < 0.1)
				{
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
	activated = true;
}

void SpawnJump::DrawImGuiControls()
{

}