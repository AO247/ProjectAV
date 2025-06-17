#pragma once
#include "Node.h"
#include "Component.h"
#include "Components.h"
#include "CMath.h"
#include "Sprite.h"
#include "Button.h"

class Node;


class UpgradeHandler : public Component
{
public:
	UpgradeHandler(Node* owner, Window& window);
	virtual ~UpgradeHandler() = default;

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;

	PlayerController* playerController = nullptr;

	struct cardData
	{
		std::unique_ptr<Sprite> level0;
		std::unique_ptr<Sprite> level1;
		std::unique_ptr<Sprite> level2;
		int count = 0;

	};
	int ability1 = 0;
	int ability2 = 0;
	int dash = 0;
	int jump = 0;

	int currentUpgrade1 = 0;
	int currentUpgrade2 = 0;
	int currentUpgrade3 = 0;

	float basicJumpForce = 0.0f;
	float basicDashForce = 0.0f;
	float basicAbility1Force = 0.0f;
	float basicAbility2Force = 0.0f;

	Node* ability1Node = nullptr;
	Node* ability2Node = nullptr;
	Node* ability3Node = nullptr;
	Node* ability4Node = nullptr;
	Node* ability5Node = nullptr;
	Node* ability6Node = nullptr;
	int slower = 0;
	bool upgradeMenuOpen = false;
	bool upgraded = false;
	bool end = false;
	float timer = 0.0f;
	float missclickTimer = 0.0f;

	void ShowUpgradeMenu();
	void SetBasicValues();
	void RandomUpgrades();
	void ResetUpgrades();
	void ApplyUpgrade(int upgradeIndex);
	void DrawUpgradeMenu();
	void CloseUpgradeMenu();
	std::unique_ptr<Sprite> cardSprite1;
	std::unique_ptr<Sprite> cardSprite2;
	std::unique_ptr<Sprite> cardSprite3;
	std::unique_ptr<Sprite> cardSprite4;
	std::unique_ptr<Sprite> ability4Gif;
	std::unique_ptr<Sprite> heart1Sprite;
	std::unique_ptr<Sprite> heart2Sprite;
	std::unique_ptr<Sprite> heart3Sprite;
	Vector4 safeValues = Vector4::Zero;
	Sprite* selectedCardSprite = nullptr;
	Sprite* card1 = nullptr;
	Sprite* card2 = nullptr;
	Sprite* card3 = nullptr;
	int card1Pos = 200;
	int card2Pos = 800;
	int card3Pos = 1400;

	std::unique_ptr<Button> testButton1;
	std::unique_ptr<Button> testButton2;
	std::unique_ptr<Button> testButton3;
private:
	Window& wnd;
};