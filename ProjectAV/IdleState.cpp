#include "IdleState.h"
#include "StateMachine.h"
#include "Node.h" // Include Node to get other components

#include <Windows.h> // For OutputDebugStringA
#include <DirectXMath.h> // For DirectX::XMFLOAT3 operations
#include <SimpleMath.h>
namespace dx = DirectX;
namespace sm = DirectX::SimpleMath;
IdleState::IdleState(StateMachine* pOwner) : State()
{
}

void IdleState::Enter(StateMachine* pOwner)
{
    OutputDebugStringA("Entering IDLE State\n");
    time = 0.0f;
}

void IdleState::Update(StateMachine* pOwner, float dt)
{

    dx::XMFLOAT3 ownerPos = pOwner->GetOwner()->GetWorldPosition();
    dx::XMFLOAT3 playerPos = pOwner->pPlayerNode->GetWorldPosition();

    sm::Vector3 vOwner(ownerPos.x, ownerPos.y, ownerPos.z);
    sm::Vector3 vPlayer(playerPos.x, playerPos.y, playerPos.z);

    if (vOwner.Distance(vOwner, vPlayer) < pOwner->followDistance) // Example threshold for "in range"
    {
        pOwner->RequestStateChange(StateType::FOLLOW);
    }

}

void IdleState::Exit(StateMachine* pOwner)
{
    OutputDebugStringA("Exiting IDLE State\n");
}