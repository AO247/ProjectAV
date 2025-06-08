#include "UpgradeHandler.h"
namespace dx = DirectX;
UpgradeHandler::UpgradeHandler(Node* owner, Window& window)
	: Component(owner), wnd(window)
{
	cardSprite1 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		80,               // int x  
		70,                // int y  
		350,               // size x  
		600,                // size y  
		L"Images\\card.png"
	);
	cardSprite2 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		475,               // int x  
		70,                // int y  
		350,               // size x  
		600,                // size y  
		L"Images\\card.png"
	);
	cardSprite3 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		870,               // int x  
		70,                // int y  
		350,               // size x  
		600,                // size y  
		L"Images\\card.png"
	);

	heart1Sprite = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		100,                // int x  
		360,                // int y  
		50,            // int width
		50,           // int height
		L"Images\\heart.png"
	);

	heart2Sprite = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		495,                // int x  
		360,                // int y (center Y)
		50,            // int width
		50,           // int height
		L"Images\\heart.png"
	);

	heart3Sprite = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),      // ID3D11Device*
		890,                // int x (center X)
		360,                // int y (center Y)
		50,            // int width
		50,           // int height
		L"Images\\heart.png"
	);
}


void UpgradeHandler::Update(float dt)
{
	if (upgraded && !end)
	{
		if (slower == 10)
		{
			bool flag = false;
			if (selectedCardSprite->x_ < 406 || selectedCardSprite->x_ > 414)
			{
				selectedCardSprite->x_ += (410 - selectedCardSprite->x_) / 3;
				flag = true;
			}
			if (selectedCardSprite->y_ < 12 || selectedCardSprite->y_ > 18)
			{
				selectedCardSprite->y_ += (15 -selectedCardSprite->y_) / 3;
				flag = true;
			}
			if (selectedCardSprite->width_ <= 440)
			{
				selectedCardSprite->width_ += 8;
				flag = true;
			}
			if (selectedCardSprite->height_ <= 690)
			{
				selectedCardSprite->height_ += 8;
				flag = true;
			}
			if (!flag)
			{
				timer = 1.5f;
				end = true;
			}
			slower = 0;
		}
		else {
			slower++;
		}
	}
	if (timer > 0.0f)
	{
		timer -= dt;
	}
	else if(end)
	{
		timer = 0.0f;
		CloseUpgradeMenu();
	}
	if (waitForGrounded)
	{
		ShowUpgradeMenu();
	}
}

void UpgradeHandler::ShowUpgradeMenu()
{
	if (upgradeMenuOpen) return;
	if (!playerController->grounded)
	{
		waitForGrounded = true;
		return;
	}
	PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(playerController->rigidbody->GetBodyID(), Vec3(0.0f,0.0f,0.0f));
	waitForGrounded = false;
	upgradeMenuOpen = true;
	end = false;
	upgraded = false;
	wnd.EnableCursor();
	wnd.mouse.DisableRaw();
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
	wnd.mouse.EnableRaw();
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
			cardSprite1->Draw(wnd.Gfx().GetContext());
			cardSprite2->Draw(wnd.Gfx().GetContext());
			cardSprite3->Draw(wnd.Gfx().GetContext());
			heart1Sprite->Draw(wnd.Gfx().GetContext());
			heart2Sprite->Draw(wnd.Gfx().GetContext());
			heart3Sprite->Draw(wnd.Gfx().GetContext());
		}
	}

}
void UpgradeHandler::RandomUpgrades()
{
	currentUpgrade1 = rand() % 4;
	currentUpgrade2 = rand() % 4;
	while(currentUpgrade1 == currentUpgrade2) // Ensure two different upgrades are chosen
	{
		currentUpgrade2 = rand() % 4;
	}
	currentUpgrade3 = rand() % 4;
	while(currentUpgrade3 == currentUpgrade1 || currentUpgrade3 == currentUpgrade2) // Ensure three different upgrades are chosen
	{
		currentUpgrade3 = rand() % 4;
	}

	//400 425 450 475
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
		selectedCardSprite = cardSprite1.get();
	}
	else if (upgradeIndex == 1)
	{
		x = currentUpgrade2;
		selectedCardSprite = cardSprite2.get();
	}
	else if (upgradeIndex == 2)
	{
		x = currentUpgrade3;
		selectedCardSprite = cardSprite3.get();
	}
	switch (x)
	{
	case 0: // Upgrade Ability1 Force
		ability1Node->GetComponent<Ability1>()->force += 200.0f;
		ability1++;
		break;
	case 1: // Upgrade Ability2 Force
		ability2Node->GetComponent<Ability2>()->force += 200.0f;
		ability2++;
		break;
	case 2: // Upgrade Dash Force
		playerController->dashForce += 20.0f;
		dash++;
		break;
	case 3: // Upgrade Jump Force
		playerController->jumpForce += 15.0f;
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
	playerController->jumpForce = basicJumpForce;
}
void UpgradeHandler::SetBasicValues()
{
	basicAbility1Force = ability1Node->GetComponent<Ability1>()->force;
	basicAbility2Force = ability2Node->GetComponent<Ability2>()->force;
	basicDashForce = playerController->dashForce;
	basicJumpForce = playerController->jumpForce;
}

void UpgradeHandler::DrawImGuiControls()
{
	ImGui::InputFloat("Basic Ability1 Force", &basicAbility1Force);
	ImGui::InputFloat("Basic Ability2 Force", &basicAbility2Force);
	ImGui::InputFloat("Basic Dash Force", &basicDashForce);
	ImGui::InputFloat("Basic Jump Force", &basicJumpForce);
	ImGui::Checkbox("Upgrade Menu Open", &upgradeMenuOpen);
}