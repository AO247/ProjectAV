#include "Island.h"
#include "CMath.h"


Island::Island(Node* owner)
	: Component(owner)
{

}

void Island::Rotate()
{
    int randomRotate = rand()% 4;
    randomRotate = randomRotate * 90;
	pOwner->SetLocalRotation(DirectX::XMFLOAT3(0.0f, to_rad(randomRotate), 0.0f));
	OutputDebugStringA(("\nRotating island: " + pOwner->GetName() + " o " + std::to_string(randomRotate) + " stopni " + std::to_string(to_rad(randomRotate))).c_str());
    if (randomRotate == 90)
    {
        Vector3 pos = upPoint->GetLocalPosition();;
        upPoint->SetLocalPosition(leftPoint->GetLocalPosition());
		leftPoint->SetLocalPosition(downPoint->GetLocalPosition());
		downPoint->SetLocalPosition(rightPoint->GetLocalPosition());
		rightPoint->SetLocalPosition(pos);
    }
    else if (randomRotate == 180)
    {
        Vector3 pos = upPoint->GetLocalPosition();
		upPoint->SetLocalPosition(downPoint->GetLocalPosition());
		downPoint->SetLocalPosition(pos);
		pos = leftPoint->GetLocalPosition();
		leftPoint->SetLocalPosition(rightPoint->GetLocalPosition());
		rightPoint->SetLocalPosition(pos);
    }
    else if (randomRotate == 270)
    {
        Vector3 pos = upPoint->GetLocalPosition();
		upPoint->SetLocalPosition(rightPoint->GetLocalPosition());
		rightPoint->SetLocalPosition(downPoint->GetLocalPosition());
		downPoint->SetLocalPosition(leftPoint->GetLocalPosition());
		leftPoint->SetLocalPosition(pos);
    }

}