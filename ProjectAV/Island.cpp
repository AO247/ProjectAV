#include "Island.h"
#include "CMath.h"


Island::Island(Node* owner)
	: Component(owner)
{

}

void Island::Rotate(int rot)
{
    int randomRotate = rand()% 4;
    randomRotate = randomRotate * 90;
	if (rot != 0) randomRotate = rot;
	pOwner->SetLocalRotation(DirectX::XMFLOAT3(0.0f, to_rad(randomRotate), 0.0f));
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