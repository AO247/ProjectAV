#include "IntersectData.h"

bool IntersectData::GetDoesIntersect()
{
    return doesIntersect;
}

float IntersectData::GetDistance()
{
    return direction.Length();
}

Vector3& IntersectData::GetDirection()
{
    return direction;
}