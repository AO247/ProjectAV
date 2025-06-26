#include "UpgradeHandler.h"
namespace dx = DirectX;
UpgradeHandler::UpgradeHandler(Node* owner, Window& window)
	: Component(owner), wnd(window)
{
	ability1.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,             
		L"Images\\karty\\push_1.gif"
	));
	ability1.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\push_2.gif"
	));

	ability1.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\string_evolve.gif"
	));
	ability1.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\gravitygun_evolve.gif"
	));




	ability2.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\toss_1.gif"
	));
	ability2.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\toss_2.gif"
	));
	ability2.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\toss_evolve.gif"
	));

	dash.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\dash_1.gif"
	));
	dash.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\dash_2.gif"
	));
	dash.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\dash_evolve.gif"
	));

	jump.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\doublejump_1.gif"
	));
	jump.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\doublejump_2.gif"
	));
	jump.cardSprites.push_back(std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\karty\\jump_evolve.gif"
	));


	testButton1 = std::make_unique<Button>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		card1Pos, 70, 350, 600,        
		L" ",         
		L"myfile.spritefont" 
	);
	testButton2 = std::make_unique<Button>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(), 
		card2Pos, 70, 350, 600,        
		L" ",           
		L"myfile.spritefont"
	);
	testButton3 = std::make_unique<Button>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		card3Pos, 70, 350, 600,       
		L" ",         
		L"myfile.spritefont" 
	);
}


void UpgradeHandler::Update(float dt)
{
	if (upgradeMenuOpen && !upgraded && missclickTimer < 0.0f)
	{

		const auto mousePos = wnd.mouse.GetPos();
		if (testButton1->IsClicked(mousePos.first, mousePos.second) && card1 != nullptr && cardData1 != nullptr)
		{
			if (wnd.mouse.IsLeftPressed()) {
				selectedCardSprite = card1;
				selectedCardData = cardData1;
				ApplyUpgrade(0);
			}
			card1->Update(dt);
		}
		else if (testButton2->IsClicked(mousePos.first, mousePos.second) && card2 != nullptr && cardData2 != nullptr)
		{
			if (wnd.mouse.IsLeftPressed()) {
				selectedCardSprite = card2;
				selectedCardData = cardData2;
				ApplyUpgrade(1);
			}
			card2->Update(dt);

		}
		else if (testButton3->IsClicked(mousePos.first, mousePos.second) && card3 != nullptr && cardData3 != nullptr)
		{
			if (wnd.mouse.IsLeftPressed()) {
				selectedCardSprite = card3;
				selectedCardData = cardData3;
				ApplyUpgrade(2);
			}
			card3->Update(dt);

		}
		if (card1 == nullptr && card2 == nullptr && card3 == nullptr)
		{
			CloseUpgradeMenu();
		}
	}
	static float animationTime = 0.0f;
	static int startX = 0, startY = 0, startW = 0, startH = 0;
	static int targetX = 0, targetY = 0, targetW = 0, targetH = 0;

	if (upgraded && !end)
	{
		if (animationTime == 0.0f)
		{
			startX = selectedCardSprite->x_;
			startY = selectedCardSprite->y_;
			startW = selectedCardSprite->width_;
			startH = selectedCardSprite->height_;

			int screenW = wnd.Gfx().GetWidth();
			int screenH = wnd.Gfx().GetHeight();
			targetW = 500; 
			targetH = 800;
			targetX = (screenW - targetW) / 2;
			targetY = (screenH - targetH) / 2;
		}

		animationTime += dt;
		float t = std::min(animationTime / 1.5f, 1.0f);

		selectedCardSprite->x_ = static_cast<int>(startX + (targetX - startX) * t);
		selectedCardSprite->y_ = static_cast<int>(startY + (targetY - startY) * t);
		selectedCardSprite->width_ = static_cast<int>(startW + (targetW - startW) * t);
		selectedCardSprite->height_ = static_cast<int>(startH + (targetH - startH) * t);

		if (t >= 1.0f)
		{
			timer = 1.5f;
			end = true;
			animationTime = 0.0f;
		}
	}
	if (timer > 0.0f)
	{
		timer -= dt;
	}
	else if (end)
	{
		timer = 0.0f;
		CloseUpgradeMenu();
	}
	if (missclickTimer > 0.0f)
	{
		missclickTimer -= dt;
	}
}

void UpgradeHandler::ShowUpgradeMenu()
{

	if (upgradeMenuOpen) return;
	missclickTimer = 0.3f;
	PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerController->rigidbody->GetBodyID(), Vec3(0.0f,0.0f,0.0f));
	upgradeMenuOpen = true;
	end = false;
	upgraded = false;
	wnd.EnableCursor();
	wnd.mouse.DisableRawInput();
	RandomUpgrades();
}
void UpgradeHandler::CloseUpgradeMenu()
{
	if (selectedCardSprite != nullptr)
	{
		selectedCardSprite->x_ = safeValues.x;
		selectedCardSprite->y_ = safeValues.y;
		selectedCardSprite->width_ = safeValues.z;
		selectedCardSprite->height_ = safeValues.w;
	}
	selectedCardSprite == nullptr;
	upgradeMenuOpen = false;
	upgraded = false;
	end = false;
	cardData1 = nullptr;
	cardData2 = nullptr;
	cardData3 = nullptr;
	card1 = nullptr;
	card2 = nullptr;
	card3 = nullptr;
	wnd.DisableCursor();
	wnd.mouse.EnableRawInput();
}
void UpgradeHandler::DrawUpgradeMenu()
{
	if (upgradeMenuOpen)
	{
		if (upgraded)
		{
			selectedCardSprite->Draw(wnd.Gfx().GetContext());
		}
		else
		{
			if(card1 != nullptr)
				card1->Draw(wnd.Gfx().GetContext());
			
			if (card2 != nullptr)
				card2->Draw(wnd.Gfx().GetContext());
			
			if (card3 != nullptr)
				card3->Draw(wnd.Gfx().GetContext());
			
		}
	}

}
void UpgradeHandler::RandomUpgrades()
{

	std::vector<cardData*> cards;
	int currentUpgrade1 = -1, currentUpgrade2 = -1, currentUpgrade3 = -1;
	if (ability1.count <2)
	{
		cards.push_back(&ability1);
	}
	if (ability2.count < 2)
	{
		cards.push_back(&ability2);
	}
	if (dash.count < 2)
	{
		cards.push_back(&dash);
	}
	if (jump.count < 2)
	{
		cards.push_back(&jump);
	}

	if (ability1.count == 2)
	{
		if(cards.size() > 0 )
			currentUpgrade3 = rand() % cards.size();

		cardData1 = &ability1;
		card1 = ability1.cardSprites[2].get();

		cardData2 = &ability1;
		card2 = ability1.cardSprites[3].get();

		if (currentUpgrade3 > -1)
		{
			cardData3 = cards[currentUpgrade3];
			card3 = cardData3->cardSprites[cards[currentUpgrade3]->count].get();
		}
	}
	else if (ability2.count == 2)
	{
		if (cards.size() > 0)
		{
			currentUpgrade1 = rand() % cards.size();
			currentUpgrade3 = rand() % cards.size();
			int count = 0;
			while (currentUpgrade1 == currentUpgrade3 && count < 15)
			{
				currentUpgrade3 = rand() % cards.size();
				count++;
			}
			if (currentUpgrade1 == currentUpgrade3)
				currentUpgrade3 = -1;
		}

		if (currentUpgrade1 > -1)
		{
			cardData1 = cards[currentUpgrade1];
			card1 = cardData1->cardSprites[cards[currentUpgrade1]->count].get();
		}

		cardData2 = &ability2;
		card2 = ability2.cardSprites[2].get();

		if (currentUpgrade3 > -1)
		{
			cardData3 = cards[currentUpgrade3];
			card3 = cardData3->cardSprites[cards[currentUpgrade3]->count].get();
		}
	}
	else if (dash.count == 2)
	{
		if (cards.size() > 0)
		{
			currentUpgrade1 = rand() % cards.size();
			currentUpgrade3 = rand() % cards.size();
			int count = 0;
			while (currentUpgrade1 == currentUpgrade3 && count < 15)
			{
				currentUpgrade3 = rand() % cards.size();
				count++;
			}
			if (currentUpgrade1 == currentUpgrade3)
				currentUpgrade3 = -1;
		}

		if (currentUpgrade1 > -1)
		{
			cardData1 = cards[currentUpgrade1];
			card1 = cardData1->cardSprites[cards[currentUpgrade1]->count].get();
		}

		cardData2 = &dash;
		card2 = dash.cardSprites[2].get();

		if (currentUpgrade3 > -1)
		{
			cardData3 = cards[currentUpgrade3];
			card3 = cardData3->cardSprites[cards[currentUpgrade3]->count].get();
		}

	}
	else if (jump.count == 2)
	{
		if (cards.size() > 0)
		{
			currentUpgrade1 = rand() % cards.size();
			currentUpgrade3 = rand() % cards.size();
			int count = 0;
			while (currentUpgrade1 == currentUpgrade3 && count < 15)
			{
				currentUpgrade3 = rand() % cards.size();
				count++;
			}
			if (currentUpgrade1 == currentUpgrade3)
				currentUpgrade3 = -1;
		}

		if (currentUpgrade1 > -1)
		{
			cardData1 = cards[currentUpgrade1];
			card1 = cardData1->cardSprites[cards[currentUpgrade1]->count].get();
		}

		cardData2 = &jump;
		card2 = jump.cardSprites[2].get();

		if (currentUpgrade3 > -1)
		{
			cardData3 = cards[currentUpgrade3];
			card3 = cardData3->cardSprites[cards[currentUpgrade3]->count].get();
		}
	}
	else
	{
		if (cards.size() > 0)
		{
			currentUpgrade1 = rand() % cards.size();
			currentUpgrade2 = rand() % cards.size();
			int count = 0;
			while (currentUpgrade1 == currentUpgrade2 && count < 15)
			{
				currentUpgrade2 = rand() % cards.size();
				count++;
			}
			if (currentUpgrade1 == currentUpgrade2)
				currentUpgrade2 = -1;
			currentUpgrade3 = rand() % cards.size();
			count = 0;
			while ((currentUpgrade3 == currentUpgrade1 || currentUpgrade3 == currentUpgrade2) && count < 15)
			{
				currentUpgrade3 = rand() % cards.size();
				count++;
			}
			if (currentUpgrade1 == currentUpgrade3 || currentUpgrade2 == currentUpgrade3)
				currentUpgrade3 = -1;
		}
		if (currentUpgrade1 > -1)
		{
			cardData1 = cards[currentUpgrade1];
			card1 = cardData1->cardSprites[cards[currentUpgrade1]->count].get();
		}
		if (currentUpgrade2 > -1)
		{
			cardData2 = cards[currentUpgrade2];
			card2 = cardData2->cardSprites[cards[currentUpgrade2]->count].get();
		}
		if (currentUpgrade3 > -1)
		{
			cardData3 = cards[currentUpgrade3];
			card3 = cardData3->cardSprites[cards[currentUpgrade3]->count].get();
		}
	}
	if(card1 != nullptr)
		card1->x_ = card1Pos;

	if (card2 != nullptr)
		card2->x_ = card2Pos;

	if (card3 != nullptr)
		card3->x_ = card3Pos;
}
void UpgradeHandler::ApplyUpgrade(int upgradeIndex)
{
	if (upgraded) return;
	
	if (selectedCardData == &ability1)
	{
		if (ability1.count == 2)
		{
			if (upgradeIndex == 0)
			{
				playerController->abilitySlot1 = ability4Node;
			}
			else if (upgradeIndex == 1)
			{
				playerController->abilitySlot1 = ability6Node;
			}
		}
		else
		{
			ability1Node->GetComponent<Ability1>()->force += 200.0f;
		}
		ability1.count++;

	}
	else if (selectedCardData == &ability2)
	{
		if (ability2.count == 2)
		{
			if (upgradeIndex == 1)
			{
				playerController->abilitySlot2 = ability5Node;
			}
		}
		else 
		{
			ability2Node->GetComponent<Ability2>()->force += 200.0f;
		}
		ability2.count++;
	}
	else if (selectedCardData == &dash)
	{
		if (dash.count == 2)
		{
			playerController->evolvedDash = true;
		}
		else
		{
			playerController->dashForce += 20.0f;
		}
		dash.count++;
	}
	else if (selectedCardData == &jump)
	{
		if (jump.count == 2)
		{
			playerController->enableFallPush = true;
		}
		else
		{
			playerController->secondJumpForce += 10.0f;
		}
		jump.count++;
	}

	safeValues.x = selectedCardSprite->x_;
	safeValues.y = selectedCardSprite->y_;
	safeValues.z = selectedCardSprite->width_;
	safeValues.w = selectedCardSprite->height_;
	upgraded = true;
}
void UpgradeHandler::ResetUpgrades()
{
	playerController->abilitySlot1 = ability1Node;
	playerController->abilitySlot2 = ability2Node;
	playerController->evolvedDash = false;
	playerController->enableFallPush = false;

	ability1.count = 0;
	ability2.count = 0;
	dash.count = 0;
	jump.count = 0;

	ability1Node->GetComponent<Ability1>()->force = basicAbility1Force;
	ability2Node->GetComponent<Ability2>()->force = basicAbility2Force;
	playerController->dashForce = basicDashForce;
	playerController->secondJumpForce = basicJumpForce;
}
void UpgradeHandler::SetBasicValues()
{
	basicAbility1Force = ability1Node->GetComponent<Ability1>()->force;
	basicAbility2Force = ability2Node->GetComponent<Ability2>()->force;
	basicDashForce = playerController->dashForce;
	basicJumpForce = playerController->secondJumpForce;
}

void UpgradeHandler::DrawImGuiControls()
{
	ImGui::InputFloat("Basic Ability1 Force", &basicAbility1Force);
	ImGui::InputFloat("Basic Ability2 Force", &basicAbility2Force);
	ImGui::InputFloat("Basic Dash Force", &basicDashForce);
	ImGui::InputFloat("Basic Jump Force", &basicJumpForce);
	ImGui::Checkbox("Upgrade Menu Open", &upgradeMenuOpen);
}