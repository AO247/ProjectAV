#include "UpgradeHandler.h"
namespace dx = DirectX;
UpgradeHandler::UpgradeHandler(Node* owner, Window& window)
	: Component(owner), wnd(window)
{
	cardSprite1 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),

		80,              
		180,             
		350,             
		600,             
		L"Images\\a1Card.png"
	);
	cardSprite2 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		475,              
		180,              
		350,              
		600,              
		L"Images\\a2Card.png"
	);
	cardSprite3 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		870,             
		180,             
		350,             
		600,             
		L"Images\\dashCard.png"
	);
	cardSprite4 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		870,              
		180,              
		350,              
		600,              
		L"Images\\jumpCard.png"
	);

	heart1Sprite = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		100,       
		360,       
		50,        
		50,        
		L"Images\\heart.png"
	);

	heart2Sprite = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		495,      
		360,      
		50,       
		50,       
		L"Images\\heart.png"
	);

	heart3Sprite = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),    
		wnd.Gfx().GetContext(),
		890,        
		360,        
		50,         
		50,         
		L"Images\\heart.png"
	);

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

	ability4Gif = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		22, 34, 190, 337, 
		L"Images\\ability4.gif"
	);

}


void UpgradeHandler::Update(float dt)
{
	if (upgradeMenuOpen && !upgraded && missclickTimer < 0.0f)
	{
		if (wnd.mouse.IsLeftPressed()) {
			const auto mousePos = wnd.mouse.GetPos();
			if (testButton1->IsClicked(mousePos.first, mousePos.second))
			{
				ApplyUpgrade(0);
			}
			else if (testButton2->IsClicked(mousePos.first, mousePos.second))
			{
				ApplyUpgrade(1);
			}
			else if (testButton3->IsClicked(mousePos.first, mousePos.second))
			{
				ApplyUpgrade(2);
			}
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
	selectedCardSprite->x_ = safeValues.x;
	selectedCardSprite->y_ = safeValues.y;
	selectedCardSprite->width_ = safeValues.z;
	selectedCardSprite->height_ = safeValues.w;
	upgradeMenuOpen = false;
	upgraded = false;
	end = false;
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

			card1->Draw(wnd.Gfx().GetContext());
			card2->Draw(wnd.Gfx().GetContext());
			card3->Draw(wnd.Gfx().GetContext());
			ability4Gif->Draw(wnd.Gfx().GetContext());
			
		}
	}

}
void UpgradeHandler::RandomUpgrades()
{
	currentUpgrade1 = rand() % 4;
	currentUpgrade2 = rand() % 4;
	while(currentUpgrade1 == currentUpgrade2)
	{
		currentUpgrade2 = rand() % 4;
	}
	currentUpgrade3 = rand() % 4;
	while(currentUpgrade3 == currentUpgrade1 || currentUpgrade3 == currentUpgrade2) 
	{
		currentUpgrade3 = rand() % 4;
	}
	switch (currentUpgrade1)
	{
	case 0: 
		card1 = cardSprite1.get();
		break;
	case 1: 
		card1 = cardSprite2.get();
		break;
	case 2: 
		card1 = cardSprite3.get();
		break;
	case 3: 
		card1 = cardSprite4.get();
		break;
	}
	card1->x_ = card1Pos;

	switch (currentUpgrade2)
	{
	case 0: 
		card2 = cardSprite1.get();
		break;
	case 1: 
		card2 = cardSprite2.get();
		break;
	case 2: 
		card2 = cardSprite3.get();
		break;
	case 3: 
		card2 = cardSprite4.get();
		break;
	}
	card2->x_ = card2Pos;

	switch (currentUpgrade3)
	{
	case 0: 
		card3 = cardSprite1.get();
		break;
	case 1: 
		card3 = cardSprite2.get();
		break;
	case 2: 
		card3 = cardSprite3.get();
		break;
	case 3: 
		card3 = cardSprite4.get();
		break;
	}
	card3->x_ = card3Pos;


	heart1Sprite->x_ = 10 + cardSprite1->x_ + currentUpgrade1 * 90;
	heart2Sprite->x_ = 10 + cardSprite2->x_ + currentUpgrade2 * 90;
	heart3Sprite->x_ = 10 + cardSprite3->x_ + currentUpgrade3 * 90;
}
void UpgradeHandler::ApplyUpgrade(int upgradeIndex)
{
	if (upgraded) return;
	int x = 0;
	if (upgradeIndex == 0)
	{
		x = currentUpgrade1;
		selectedCardSprite = card1;
	}
	else if (upgradeIndex == 1)
	{
		x = currentUpgrade2;
		selectedCardSprite = card2;
	}
	else if (upgradeIndex == 2)
	{
		x = currentUpgrade3;
		selectedCardSprite = card3;
	}
	switch (x)
	{
	case 0: 
		ability1Node->GetComponent<Ability1>()->force += 200.0f;
		ability1++;
		break;
	case 1: 
		ability2Node->GetComponent<Ability2>()->force += 200.0f;
		ability2++;
		break;
	case 2: 
		playerController->dashForce += 20.0f;
		dash++;
		break;
	case 3:
		playerController->jumpForce += 10.0f;
		jump++;
		break;
	}
	upgraded = true;
	safeValues.x = selectedCardSprite->x_;
	safeValues.y = selectedCardSprite->y_;
	safeValues.z = selectedCardSprite->width_;
	safeValues.w = selectedCardSprite->height_;
}
void UpgradeHandler::ResetUpgrades()
{
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