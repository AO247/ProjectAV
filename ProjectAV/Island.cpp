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

    if (randomRotate == 90)
    {
        Node* tym = upPoint;
        upPoint = leftPoint;
        leftPoint = downPoint;
        downPoint = rightPoint;
        rightPoint = tym;
    }
    else if (randomRotate == 180)
    {
        Node* tym = upPoint;
        upPoint = downPoint;
        downPoint = tym;
        tym = rightPoint;
        rightPoint = leftPoint;
        leftPoint = tym;
    }
    else if (randomRotate == 270)
    {
        Node* tym = upPoint;
        upPoint = rightPoint;
        rightPoint = downPoint;
        downPoint = leftPoint;
        leftPoint = tym;
    }

}