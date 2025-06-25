#include "StoneGenerator.h"
#include <string>
#include "Components.h"
#include "PrefabManager.h"
StoneGenerator::StoneGenerator(Node* owner)
	: Component(owner)
{
}

void StoneGenerator::Update(float dt)
{
	if (!active) return;
	timer += dt;
	if (timer >= dropRate)
	{
		Vector3 pos = pOwner->GetLocalPosition();
		pos.y += 350.0f;
		float xOffset = (rand() % 440) - 240;
		float zOffset = (rand() % 440) - 240;
		pos.x += xOffset;
		pos.z += zOffset;
		Vec3 direction = Vec3(0.0f, -400.0f, 0.0f);
		RRayCast ray= RRayCast(
			RVec3(pos.x, pos.y, pos.z),
			direction
		);
		RayCastResult result;
		if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(ray, result,
			IgnoreSpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::TRIGGER),
			IgnoreSpecifiedObjectLayerFilter(Layers::TRIGGER)))
		{
			RRayCast rayUp = RRayCast(
				RVec3(pos.x, pos.y, pos.z),
				direction
			);
			RayCastResult resultUp;
			if (PhysicsCommon::physicsSystem->GetNarrowPhaseQuery().CastRay(rayUp, resultUp,
				SpecifiedBroadPhaseLayerFilter(BroadPhaseLayers::GROUND),
				SpecifiedObjectLayerFilter(Layers::GROUND)))
			{
				Vec3 hitPos = rayUp.mOrigin + rayUp.mDirection * resultUp.mFraction;
				pos = Vector3(hitPos.GetX(), hitPos.GetY(), hitPos.GetZ());
				pos.y -= 8.0f;
			}
			else
			{
				pos.y = 390.0f;
			}
			timer = 0.0f;
			PrefabManager::InstantiateThrowable(pOwner, pos, 0.4f);
		}
	}
}

void StoneGenerator::DrawImGuiControls()
{
	ImGui::InputFloat("DropRate: %f", &dropRate);
	ImGui::Checkbox("Active", &active);
}

