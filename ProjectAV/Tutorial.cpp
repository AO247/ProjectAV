#include "Tutorial.h"

namespace dx = DirectX;
Tutorial::Tutorial(Node* owner, Window* window, Node* player)
	: Component(owner), wnd(window), player(player)
{


	note1 = std::make_unique<Sprite>(
		wnd->Gfx().GetDevice(),
		wnd->Gfx().GetContext(),
		80,
		180,
		350,
		600,
		L"Images\\note1.png"
	);
    checkpoints.push_back({ 8.5f, -17.2f, -422.7f });
    checkpoints.push_back({ 72.5f, -19.0f, -465.1f });
    checkpoints.push_back({ 155.0f, -0.5f, -412.3f });
    checkpoints.push_back({ 221.3f, -0.8f, -366.0f });
    checkpoints.push_back({ 155.8f, -17.3f, -239.0f });
    checkpoints.push_back({ 69.3f, -16.4f, -220.7f });
	checkpoints.push_back({ 20.5f, 3.2f, -105.1f});

	player->SetLocalPosition(checkpoints[0]);

}


void Tutorial::Update(float dt)
{
	if (completed) return;
	EnemyHandler();

	if(player->GetLocalPosition().y < checkpoints[currentCheckpointIndex].y - 30.0f)
	{
		player->SetLocalPosition(checkpoints[currentCheckpointIndex]);
		playerDeathCount++;
	}
	if ((player->GetLocalPosition() - checkpoints[currentCheckpointIndex + 1]).Length() < 10.0f)
	{
		currentCheckpointIndex++;
		currentStage++;
		timer = 0.0f;
		playerDeathCount = 0;
		currentNote = nullptr;
		if (currentCheckpointIndex >= checkpoints.size() - 1)
		{
			completed = true;
		}
	}
	if (currentStage == 1)
	{
		Stage1(dt);
	}
	else if (currentStage == 2)
	{
		Stage2(dt);
	}
	Stage2(dt);
}

void Tutorial::Stage1(float dt)
{
	timer += dt;
	if (timer > 20.0f || playerDeathCount == 3)
	{
		currentNote = note1.get();
		//message press jump mid air to use second jump
	}

}
void Tutorial::Stage2(float dt)
{
	timer += dt;
	if (timer > 20.0f)
	{
		//currentNote = note1.get();
		//message press jump mid air to use second jump
	}
	if (stone1->GetLocalPosition().y < stone1Pos.y - 30.0f)
	{
		BodyID bid = stone1->GetComponent<Rigidbody>()->GetBodyID();
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(bid, Vec3::sZero());
		PhysicsCommon::physicsSystem->GetBodyInterface().SetAngularVelocity(bid, Vec3::sZero());
		PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(bid, Vec3(stone1Pos.x, stone1Pos.y, stone1Pos.z), EActivation::Activate);
	}
}

void Tutorial::DrawNote()
{
	if (currentNote == nullptr) return;
	
	currentNote->Draw(wnd->Gfx().GetContext());
}

void Tutorial::EnemyHandler()
{
	if (enemy1 != nullptr)
	{
		enemy1->RequestStateChange(StateType::STOP);
	}

}
void Tutorial::SetStones()
{
	stone1Pos = stone1->GetWorldPosition();
	stone2Pos = stone2->GetWorldPosition();
}
void Tutorial::DrawImGuiControls()
{
	ImGui::Text("Completed: %d", completed);
	ImGui::Text("Current Checkpoint: %d", currentCheckpointIndex);
	ImGui::Text("Timer: %.2f", timer);
	ImGui::Text("Current Stage: %d", currentStage);

}