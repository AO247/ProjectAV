#pragma once
#include "PhysicsCommon.h"

using namespace JPH;

class PhysicsDebugRenderer : DebugRenderer
{
public:
	void DrawLine(RVec3Arg inFrom, RVec3Arg inTo, ColorArg inColor) override;
	void DrawTriangle(RVec3Arg inV1, RVec3Arg inV2, RVec3Arg inV3, ColorArg inColor, ECastShadow inCastShadow) override;
};

