#include "MainMenu.h"
namespace dx = DirectX;
MainMenu::MainMenu(Node* owner, Window& window)
	: Component(owner), wnd(window)
{

	startBttn = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		(wnd.GetWidth() * 0.055f), (wnd.GetHeight() * 0.543f), (wnd.GetWidth() * 0.229f), (wnd.GetHeight() * 0.098f),
		L"Images\\start_2.png"
	);

	quitBttn = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		(wnd.GetWidth() * 0.055f), (wnd.GetHeight() * 0.673f), (wnd.GetWidth() * 0.229f), (wnd.GetHeight() * 0.098f),
		L"Images\\quit_2.png"
	);
	

	mainMenuBackground = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(), wnd.Gfx().GetContext(),
		0, 0, wnd.GetWidth(), wnd.GetHeight(),
		L"Images\\menu_9.gif"
	);

	width = wnd.GetWidth();
	height = wnd.GetHeight();
}


void MainMenu::Update(float dt)
{
	//wnd.Gfx().GetTarget()->BindAsBuffer(wnd.Gfx());
	
	//mainMenuBackground->Update(dt);
	//mainMenuBackground->Draw(wnd.Gfx().GetContext());

	int mouseX = wnd.mouse.GetPosX();
	int mouseY = wnd.mouse.GetPosY();
	bool isMouseLeftPressedThisFrame = wnd.mouse.IsLeftPressed();

	bool isLeftClick = isMouseLeftPressedThisFrame && !wasMouseLeftPressedLastFrame;

	if (mouseX >= (wnd.GetWidth() * 0.055f) && mouseX <= ((wnd.GetWidth() * 0.055f) + (wnd.GetWidth() * 0.229f)) &&
		mouseY >= (wnd.GetHeight() * 0.543f) && mouseY <= ((wnd.GetHeight() * 0.543f) + (wnd.GetHeight() * 0.098f))) {
		startBttn->Draw(wnd.Gfx().GetContext());
		if (isLeftClick) {
			onStartClick(); 
		}
	}

	if (mouseX >= (wnd.GetWidth() * 0.055f) && mouseX <= ((wnd.GetWidth() * 0.055f) + (wnd.GetWidth() * 0.229f)) &&
		mouseY >= (wnd.GetHeight() * 0.673f) && mouseY <= ((wnd.GetHeight() * 0.673f) + (wnd.GetHeight() * 0.098f))) {
		quitBttn->Draw(wnd.Gfx().GetContext());
		if (isLeftClick) {
			onQuitClick();
		}
 	}
	/*quitBttn->SetPosition(x * width, y * height);
	quitBttn->SetSize(w * width, h * height);*/

	//startBttn->Draw(wnd.Gfx().GetContext());
	wasMouseLeftPressedLastFrame = isMouseLeftPressedThisFrame;

}

void MainMenu::DrawMainMenu(float dt)
{

	//mainMenuBackground->Draw(wnd.Gfx().GetContext());
	//quitBttn->Draw(wnd.Gfx().GetContext());
	//startBttn->Draw(wnd.Gfx().GetContext()); 

}

void MainMenu::SetOnStartClick(OnClickCallback callback)
{
	onStartClick = callback;
}

void MainMenu::SetOnQuitClick(OnClickCallback callback)
{
	onQuitClick = callback;
}

void MainMenu::DrawImGuiControls()
{
	ImGui::SliderFloat("Card X", &x, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Y", &y, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Width", &w, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Height", &h, 0.0f, 1.0f);
}