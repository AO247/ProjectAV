#pragma once
#include "Window.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include "Mesh.h"
#include <set>

class App
{
public:
	App();
	// master frame / message loop
	int Go();
	~App();
private:
	void DoFrame(float dt);
	void HandleInput(float dt);
	void ShowImguiDemoWindow();
private:
	ImguiManager imgui;
	Window wnd;
	Timer timer;
	float speed_factor = 1.0f;
	bool cursorEnabled = true;
	int x = 0, y = 0;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(),"Models\\nano.gltf" };
};