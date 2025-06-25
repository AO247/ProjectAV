#include "MainMenu.h"
namespace dx = DirectX;
MainMenu::MainMenu(Node* owner, Window& window)
	: Component(owner), wnd(window)
{
	cardSprite1 = std::make_unique<Sprite>(
		wnd.Gfx().GetDevice(),
		wnd.Gfx().GetContext(),
		0, 180, 350, 600,
		L"Images\\a1Card.png"
	);
	width = wnd.GetWidth();
	height = wnd.GetHeight();
}


void MainMenu::Update(float dt)
{
	cardSprite1->SetPosition(x * width, y * height);
	cardSprite1->SetSize(w * width, h * height);
}

void MainMenu::DrawMainMenu(float dt)
{
	cardSprite1->Draw(wnd.Gfx().GetContext());
}

void MainMenu::DrawImGuiControls()
{
	ImGui::SliderFloat("Card X", &x, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Y", &y, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Width", &w, 0.0f, 1.0f);
	ImGui::SliderFloat("Card Height", &h, 0.0f, 1.0f);
}