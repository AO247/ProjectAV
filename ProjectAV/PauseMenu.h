#pragma once
#include "Node.h"
#include "Component.h"
#include "Components.h"
#include "Window.h"
#include <DirectXMath.h>

class Node;
class PrefabManager;
class LevelGenerator;


class PauseMenu : public Component
{
public:
	PauseMenu(Node* owner, Window& window);
	virtual ~PauseMenu() = default;

	using OnClickCallback = std::function<void()>;
	void SetOnResumeClick(OnClickCallback callback);
	void SetOnQuitToMenuClick(OnClickCallback callback);

	virtual void Update(float dt) override;
	virtual void DrawImGuiControls() override;
	void DrawPauseMenu(float dt);
	std::unique_ptr<Sprite> cardSprite1;
	float w, h, x, y;
	float width, height;

	std::unique_ptr<Sprite> pauseMenuBackground;
	std::unique_ptr<Sprite> resumeBttn;
	std::unique_ptr<Sprite> backBttn;

	bool wasMouseLeftPressedLastFrame = false;


private:
	Window& wnd;
	OnClickCallback onResumeClick;
	OnClickCallback onQuitToMenuClick;
};