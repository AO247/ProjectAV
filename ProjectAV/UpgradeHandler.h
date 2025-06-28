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
		std::vector<std::unique_ptr<Sprite>> cardSprites;
		int count = 0;

	};

	cardData ability1;
	cardData ability2;
	cardData dash;
	cardData jump;



	float basicJumpForce = 0.0f;
	float basicDashForce = 0.0f;
	float basicAbility1Force = 0.0f;
	float basicAbility2Force = 0.0f;

	cardData* cardData1;
	cardData* cardData2;
	cardData* cardData3;
	cardData* selectedCardData;

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
	float screenWidth = 0.0f;
	float screenHeight = 0.0f;
	Vector2 cardSize = Vector2::Zero;

	std::unique_ptr<Button> testButton1;
	std::unique_ptr<Button> testButton2;
	std::unique_ptr<Button> testButton3;
private:
	Window& wnd;
};