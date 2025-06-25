#pragma once
#include "Node.h"
#include "Component.h"
#include "Components.h"
#include "Window.h"
#include <DirectXMath.h>
 
class Node;
class PrefabManager;
class LevelGenerator;


class MainMenu : public Component
{
public: 

	MainMenu(Node* owner, Window& window);
	virtual ~MainMenu() = default;

	using OnClickCallback = std::function<void()>;
	void SetOnStartClick(OnClickCallback callback);
	void SetOnQuitClick(OnClickCallback callback);

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void DrawMainMenu(float dt);
	std::unique_ptr<Sprite> cardSprite1;
	float w, h, x, y;
	float width, height;

	std::unique_ptr<Sprite> mainMenuBackground;
	std::unique_ptr<Sprite> quitBttn;
	std::unique_ptr<Sprite> startBttn;
	 
	bool wasMouseLeftPressedLastFrame = false; 

private:
	Window& wnd;
	OnClickCallback onStartClick;
	OnClickCallback onQuitClick;
};