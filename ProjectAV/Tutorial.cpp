#include "Tutorial.h"
#include "PrefabManager.h"
namespace dx = DirectX;
Tutorial::Tutorial(Node* owner, Window& window, Node* player)
	: Component(owner), wnd(window), player(player)
{

	note0 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note1.png"
	);
	note1 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note2.png"
	);
	note2 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note3.png"
	);
	note3 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note4.png"
	);
	note4 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note5.png"
	);
	note5 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note6.png"
	);
	note6 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note7.png"
	);
	note7 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		30,
		30,
		400,
		200,
		L"Images\\note8.png"
	);
    checkpoints.push_back({ 8.5f, -17.2f, -440.7f });
	checkpoints.push_back({ 31.7f, -17.2f, -447.9f });
    checkpoints.push_back({ 72.5f, -19.0f, -465.1f });
    checkpoints.push_back({ 155.0f, -0.5f, -412.3f });
    checkpoints.push_back({ 221.3f, -0.8f, -366.0f });
	checkpoints.push_back({ 217.3f, -17.5f, -296.1f });
    checkpoints.push_back({ 155.8f, -17.3f, -239.0f });
    checkpoints.push_back({ 69.3f, -16.4f, -220.7f });
	checkpoints.push_back({ 20.5f, 3.2f, -105.1f});



}


void Tutorial::Update(float dt)
{
	if (completed || !started) return;
	timer += dt;
	if (timer > 3.0f && !enemySpawned)
	{
		enemySpawned = true;
		enemy1 = PrefabManager::InstantiateFastEnemy(temporary, { 222.7f, -19.3f, -265.1f })->GetComponent<StateMachine>();
		enemy2 = PrefabManager::InstantiateNormalEnemy(temporary, { 95.9f, -15.0f, -240.5f })->GetComponent<StateMachine>();
		enemy3 = PrefabManager::InstantiateNormalEnemy(temporary, { 44.1f, -15.3f, -198.9f })->GetComponent<StateMachine>();
		enemy4 = PrefabManager::InstantiateShootingEnemy(temporary, { 32.2f, -15.8f, -203.6f })->GetComponent<StateMachine>();
		enemy5 = PrefabManager::InstantiateShootingEnemy(temporary, { 35.2f, -15.8f, -190.6f })->GetComponent<StateMachine>();
		enemy6 = PrefabManager::InstantiateTankEnemy(temporary, { 213.7f, -16.3f, -208.9f })->GetComponent<StateMachine>();
		enemy1->Stop(1000.0f);
		enemy2->Stop(1000.0f);
		enemy3->Stop(1000.0f);
		enemy4->Stop(1000.0f);
		enemy5->Stop(1000.0f);
		enemy6->Stop(1000.0f);

	}
	//EnemyHandler();

	if(player->GetLocalPosition().y < checkpoints[currentCheckpointIndex].y - 60.0f)
	{
		player->SetLocalPosition(checkpoints[currentCheckpointIndex]);
		playerDeathCount++;
	}
	if (currentCheckpointIndex < checkpoints.size() - 1)
	{
		if ((player->GetLocalPosition() - checkpoints[currentCheckpointIndex + 1]).Length() < 10.0f
			/*|| (player->GetLocalPosition() - checkpoints[currentCheckpointIndex + 2]).Length() < 10.0f*/)
		{
			currentCheckpointIndex++;
			currentStage++;
			timer = 0.0f;
			playerDeathCount = 0;
			currentNote = nullptr;

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
	if (currentCheckpointIndex == 1)
	{
		currentNote = note0.get();
	}
	if (currentCheckpointIndex == 2)
	{
		currentNote = note1.get();
	}
	if (currentCheckpointIndex == 3)
	{
		currentNote = note2.get();
	}
	if (currentCheckpointIndex == 5)
	{
		currentNote = note3.get();
	}
	if (currentCheckpointIndex == 6)
	{
		currentNote = note4.get();
	}
	if (currentCheckpointIndex == 7)
	{
		currentNote = note5.get();
	}
	if (currentCheckpointIndex == 7 && qPressed)
	{
		currentNote = note6.get();
	}
	if (currentCheckpointIndex == 8)
	{
		currentNote = note7.get();
	}
	if (currentCheckpointIndex >= checkpoints.size() - 1 && timer > 5.0f)
	{
		currentNote = nullptr;
		completed = true;
	}
}

void Tutorial::Stage1(float dt)
{
	//timer += dt;
	//if (timer > 20.0f || playerDeathCount == 3)
	//{
	//	currentNote = note1.get();
	//	//message press jump mid air to use second jump
	//}

}
void Tutorial::Stage2(float dt)
{
	timer += dt;
	if (timer > 20.0f)
	{
		//currentNote = note2.get();
		//message press jump mid air to use second jump
	}
	if (stone1->GetLocalPosition().y < stone1Pos.y - 30.0f)
	{
		BodyID bid = stone1->GetComponent<Rigidbody>()->GetBodyID();
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(bid, Vec3::sZero());
		PhysicsCommon::physicsSystem->GetBodyInterface().SetAngularVelocity(bid, Vec3::sZero());
		PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(bid, Vec3(stone1Pos.x, stone1Pos.y, stone1Pos.z), EActivation::Activate);
	}
	if (stone2->GetLocalPosition().y < stone2Pos.y - 30.0f)
	{
		BodyID bid = stone2->GetComponent<Rigidbody>()->GetBodyID();
		PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(bid, Vec3::sZero());
		PhysicsCommon::physicsSystem->GetBodyInterface().SetAngularVelocity(bid, Vec3::sZero());
		PhysicsCommon::physicsSystem->GetBodyInterface().SetPosition(bid, Vec3(stone2Pos.x, stone2Pos.y, stone2Pos.z), EActivation::Activate);
	}
}

void Tutorial::DrawNote()
{
	if (currentNote == nullptr) return;
	
	currentNote->Draw(wnd.Gfx().GetContext());
}
void Tutorial::Start()
{
	started = true;
	player->SetLocalPosition(checkpoints[0]);
}
void Tutorial::Reset()
{
	timer = 0.0f;
	currentCheckpointIndex = 0;
	currentStage = 0;
	playerDeathCount = 0;
	currentNote = nullptr;
	completed = false;
	enemySpawned = false;
	started = false;
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