#include "App.h"

App::App()
	:
	wnd(800, 600, "ProjectAV")
{
}

int App::Go()
{
	while (true)
	{	
		if (const auto ecode = Window::ProcessMessages())
		{
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float t = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Gfx().ClearBuffer(t, t, 1.0f);
	wnd.Gfx().DrawTestTriangle(timer.Peek(), wnd.mouse.GetPosX() / 400.0f - 1.0f, -wnd.mouse.GetPosY() / 300.0f + 1.0f);
	wnd.Gfx().EndFrame();

}