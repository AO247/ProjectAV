#pragma once
#include "Component.h"
#include "Window.h"
#include <DirectXMath.h>
#include "Components.h"
#include "Sprite.h"
class Node;


class Tutorial : public Component
{
public:
	Tutorial(Node* owner, Window& window, Node* player);
	virtual ~Tutorial() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void DrawNote();
	void SetStones();
	void Start();
	void Reset();
	Node* player;
	std::vector<Node*> enemies;
	std::vector<Node*> levels;
	std::vector<Vector3> checkpoints;
	std::unique_ptr<Sprite> note0;
	std::unique_ptr<Sprite> note1;
	std::unique_ptr<Sprite> note2;
	std::unique_ptr<Sprite> note3;
	std::unique_ptr<Sprite> note4;
	std::unique_ptr<Sprite> note5;
	std::unique_ptr<Sprite> note6;
	std::unique_ptr<Sprite> note7;
	Sprite* currentNote;
	bool enemySpawned = false;
	Node* stone1 = nullptr;
	Vector3 stone1Pos;
	Node* stone2 = nullptr;
	Vector3 stone2Pos;
	Node* temporary = nullptr;
	StateMachine* enemy1 = nullptr;
	StateMachine* enemy2 = nullptr;
	StateMachine* enemy3 = nullptr;
	StateMachine* enemy4 = nullptr;
	StateMachine* enemy5 = nullptr;


	int currentCheckpointIndex = 0;
	bool completed = false;
	int endingRemover = 0;
	int currentStage = 1;
	int playerDeathCount = 0;
	float timer = 0.0f;
	bool qPressed = false;
	bool started = false;
private:
	Window& wnd;

	void Stage1(float dt);
	void Stage2(float dt);
};