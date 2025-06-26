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
	/*if (!active) return;
	timer += dt;
	timerBakground += dt;
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
	}*/
	/*if (timerBakground > dropRateBackground)
	{
		float xdir = ((rand() % 200) - 100) / 100.0f;
		float zdir = ((rand() % 200) - 100) / 100.0f;

		Vector3 dir = Vector3(xdir, 0.0f, zdir);
		float ran = (rand() % 50) - 25;
		Vector3 pos = dir * (300.0f - ran);
		pos.y = 450.0f;
		if (stone1 == nullptr)
			stone1 = PrefabManager::InstantiateThrowable(pOwner, pos, 0.4f);
		else if (stone2 == nullptr)
			stone2 = PrefabManager::InstantiateThrowable(pOwner, pos, 0.4f);
		else if (stone3 == nullptr)
			stone3 = PrefabManager::InstantiateThrowable(pOwner, pos, 0.4f);
		else if (stone4 == nullptr)
			stone4 = PrefabManager::InstantiateThrowable(pOwner, pos, 0.4f);


		timerBakground = 0.0f;
	}
	if (stone1 != nullptr)
	{
		if(stone1->GetLocalPosition().y < -500.0f)
		{
			stone1->Destroy();
			stone1 = nullptr;
		}
	}
	if (stone2 != nullptr)
	{
		if (stone2->GetLocalPosition().y < -500.0f)
		{
			stone2->Destroy();
			stone2 = nullptr;
		}
	}
	if (stone3 != nullptr)
	{
		if (stone3->GetLocalPosition().y < -500.0f)
		{
			stone3->Destroy();
			stone3 = nullptr;
		}
	}
	if (stone4 != nullptr)
	{
		if (stone4->GetLocalPosition().y < -500.0f)
		{
			stone4->Destroy();
			stone4 = nullptr;
		}
	}*/
}

void StoneGenerator::DrawImGuiControls()
{
	ImGui::InputFloat("DropRate: %f", &dropRate);
	ImGui::InputFloat("DropRateBackGround: %f", &dropRateBackground);

	ImGui::Checkbox("Active", &active);
}

