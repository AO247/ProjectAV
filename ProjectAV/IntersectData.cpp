#include "IntersectData.h"

bool IntersectData::GetDoesIntersect()
{
    return doesIntersect;
}

float IntersectData::GetDistance()
{
    return direction.Length();
}

Vector3 IntersectData::GetDirection()
{
    return direction;
}

Vector3 IntersectData::GetCollisionPoint()
{
    return collisionPoint;
}

Vector3 IntersectData::GetCollisionPointRadiusForFirstBody()
{
    return collisionPointRadiusForFirstBody;
}

Vector3 IntersectData::GetCollisionPointRadiusForSecondBody()
{
    return collisionPointRadiusForSecondBody;
}