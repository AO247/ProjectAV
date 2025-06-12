#include "SpawnJump.h"

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
			if (!playerReady && (pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 7.0f && jumpCooldown < 0.1)
			{
				PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
				playerReady = true;
				wnd.playerLocked = true;
			}
			if (playerReady)
			{
				if ((pOwner->GetWorldPosition() - playerNode->GetWorldPosition()).Length() < 7.0f && jumpCooldown < 0.1)
				{
					PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(0.0f, 280.0f, 0.0f));
					moved = false;
					pushed = false;
					jumpCooldown = 0.5f;
				}
				if (!halfWay && playerNode->GetWorldPosition().y - pOwner->GetWorldPosition().y > 80.0f)
				{
					halfWay = true;
				}
				if (!moved && playerNode->GetWorldPosition().y > targetPosition.y + 4.0f)
				{
					Vector3 direction = (targetPosition - playerNode->GetWorldPosition());
					direction.y = 0.0f;
					direction *= 300.0f * dt;
					PhysicsCommon::physicsSystem->GetBodyInterface().AddForce(playerNode->GetComponent<Rigidbody>()->GetBodyID(), Vec3(direction.x, 0.0f, direction.z));
					pushed = true;
					if (wnd.playerLocked)
					{
						auto* cameraNode = playerNode->GetComponent<PlayerController>()->camera;
						cameraNode->SetLocalRotation(DirectX::XMFLOAT3(0.0f, targetYaw, 0.0f));
						cameraNode->GetComponent<Camera>()->xRotation = 0.0f;
						cameraNode->GetComponent<Camera>()->yRotation = targetYaw;
						wnd.playerLocked = false;
					}
					else if (!lastRotate)
					{
						auto* cameraNode = playerNode->GetComponent<PlayerController>()->camera;
						cameraNode->SetLocalRotation(DirectX::XMFLOAT3(0.0f, targetYaw, 0.0f));
						cameraNode->GetComponent<Camera>()->xRotation = 0.0f;
						cameraNode->GetComponent<Camera>()->yRotation = targetYaw;
						lastRotate = true;
					}
				}
				else if (pushed && playerNode->GetWorldPosition().y < targetPosition.y + 4.0f)
				{
					moved = true;
					rotated = false;
					halfWay = false;
					playerReady = false;
					lastRotate = false;
					pushed = false;
				}
				if (!rotated)
				{

					Vector3 direction = (targetPosition - playerNode->GetWorldPosition());
					direction.y = 0.0f;
					direction.Normalize();
					targetYaw = atan2f(direction.x, direction.z);

					auto* cameraNode = playerNode->GetComponent<PlayerController>()->camera;
					DirectX::XMFLOAT3 currentRot = cameraNode->GetLocalRotationEuler();
					float currentYaw = currentRot.y;

					float yawDifference = wrap_angle(targetYaw - currentYaw);
					float newYaw = wrap_angle(currentYaw + yawDifference * 0.05f);

					cameraNode->SetLocalRotation(DirectX::XMFLOAT3(0.0f, newYaw, 0.0f));

					constexpr float epsilon = 0.1f;
					if (std::abs(wrap_angle(newYaw - targetYaw)) < epsilon)
					{
						cameraNode->SetLocalRotation(DirectX::XMFLOAT3(0.0f, targetYaw, 0.0f));
						cameraNode->GetComponent<Camera>()->xRotation = 0.0f;
						cameraNode->GetComponent<Camera>()->yRotation = targetYaw;
						rotated = true;
					}
				}
			}
		}
	}

	if (jumpCooldown > 0.0f) jumpCooldown -= dt;
}

void SpawnJump::Activate(Vector3 targetPos)
{
	targetPosition = targetPos;
	activated = true;
}

void SpawnJump::DrawImGuiControls()
{

}