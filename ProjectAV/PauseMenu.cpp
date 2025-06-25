#include "PauseMenu.h"

namespace dx = DirectX;
PauseMenu::PauseMenu(Node* owner, Window& window)
	: Component(owner), wnd(window)
{

	resumeBttn = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		(wnd.GetWidth() * 0.354f), (wnd.GetHeight() * 0.339f), (wnd.GetWidth() * 0.296f), (wnd.GetHeight() * 0.110f),
		L"Images\\resume.png"
	);

	backBttn = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		(wnd.GetWidth() * 0.354f), (wnd.GetHeight() * 0.489f), (wnd.GetWidth() * 0.296f), (wnd.GetHeight() * 0.110f),
		L"Images\\back_to_menu.png"
	);


	pauseMenuBackground = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
		0, 0, wnd.GetWidth(), wnd.GetHeight(),
		L"Images\\pause_menu.png"
	);

	width = wnd.GetWidth();
	height = wnd.GetHeight();
}


void PauseMenu::Update(float dt)
{ 

	//mainMenuBackground->Update(dt);
	//mainMenuBackground->Draw(wnd.Gfx().GetContext());
	pauseMenuBackground->Draw(wnd.Gfx().GetContext());
	int mouseX = wnd.mouse.GetPosX();
	int mouseY = wnd.mouse.GetPosY();
	bool isMouseLeftPressedThisFrame = wnd.mouse.IsLeftPressed();

	bool isLeftClick = isMouseLeftPressedThisFrame && !wasMouseLeftPressedLastFrame;

	if (mouseX >= (wnd.GetWidth() * 0.354f) && mouseX <= ((wnd.GetWidth() * 0.354f) + (wnd.GetWidth() * 0.296f)) &&
		mouseY >= (wnd.GetHeight() * 0.339f) && mouseY <= ((wnd.GetHeight() * 0.339f) + (wnd.GetHeight() * 0.110f))) {
		resumeBttn->Draw(wnd.Gfx().GetContext());
		if (isLeftClick) {
			onResumeClick();
		}
	}

	if (mouseX >= (wnd.GetWidth() * 0.354f) && mouseX <= ((wnd.GetWidth() * 0.354f) + (wnd.GetWidth() * 0.296f)) &&
		mouseY >= (wnd.GetHeight() * 0.489f) && mouseY <= ((wnd.GetHeight() * 0.489f) + (wnd.GetHeight() * 0.110f))) {
		backBttn->Draw(wnd.Gfx().GetContext());
		if (isLeftClick) {
			onQuitToMenuClick();
		}
	}

	/*resumeBttn->SetPosition(x * width, y * height);
	resumeBttn->SetSize(w * width, h * height);*/

	 
}

void PauseMenu::DrawPauseMenu(float dt)
{

	pauseMenuBackground->Draw(wnd.Gfx().GetContext());
	resumeBttn->Draw(wnd.Gfx().GetContext());
	//startBttn->Draw(wnd.Gfx().GetContext()); 

}

void PauseMenu::DrawImGuiControls()
{
	ImGui::SliderFloat("Card X", &x, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Y", &y, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Width", &w, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Height", &h, 0.0f, 1.0f);
}

void PauseMenu::SetOnResumeClick(OnClickCallback callback)
{
	onResumeClick = callback;
}

void PauseMenu::SetOnQuitToMenuClick(OnClickCallback callback)
{
	onQuitToMenuClick = callback;
}
