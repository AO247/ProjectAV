#include "App.h"
#include "AssTest.h"
#include <memory>
#include <algorithm>
#include "CMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm;

App::App()
	:
	wnd( 800,600,"Project AV" ),
	light( wnd.Gfx() )
{
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f,3.0f / 4.0f,0.5f,40.0f ) );
}

void App::DoFrame(float dt)
{
	wnd.Gfx().BeginFrame( 0.07f,0.0f,0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );
	light.Bind( wnd.Gfx(),cam.GetMatrix() );

	const auto transform = dx::XMMatrixRotationRollPitchYaw( pos.roll,pos.pitch,pos.yaw ) *
		dx::XMMatrixTranslation( pos.x,pos.y,pos.z );
	nano.Draw( wnd.Gfx(),transform );
	light.Draw( wnd.Gfx() );
		
	// imgui windows
	light.SpawnControlWindow();
	ShowModelWindow();

	// present
	wnd.Gfx().EndFrame();
}

void App::HandleInput(float dt)
{
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}
		switch (e->GetCode())
		{
		case 'C':
			if (cursorEnabled)
			{
				wnd.DisableCursor();
				cursorEnabled = false;
			}
			else
			{
				wnd.EnableCursor();
				cursorEnabled = true;
			}
		}
	}
	if (wnd.kbd.KeyIsPressed('W'))
	{
		cam.Translate({ 0.0f, 0.0f, dt });
	}
	if (wnd.kbd.KeyIsPressed('S'))
	{
		cam.Translate({ 0.0f, 0.0f, -dt });
	}
	if (wnd.kbd.KeyIsPressed('A'))
	{
		cam.Translate({ -dt, 0.0f, 0.0f });
	}
	if (wnd.kbd.KeyIsPressed('D'))
	{
		cam.Translate({ dt, 0.0f, 0.0f });
	}

	if (!cursorEnabled)
	{
		while (const auto delta = wnd.mouse.ReadRawDelta())
		{
			cam.Rotate(delta->x, delta->y);
		}
	}
}

void App::ShowModelWindow()
{
	if( ImGui::Begin( "Model" ) )
	{
		using namespace std::string_literals;

		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&pos.roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pos.pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&pos.yaw,-180.0f,180.0f );
		
		ImGui::Text( "Position" );
		ImGui::SliderFloat( "X",&pos.x,-20.0f,20.0f );
		ImGui::SliderFloat( "Y",&pos.y,-20.0f,20.0f );
		ImGui::SliderFloat( "Z",&pos.z,-20.0f,20.0f );
	}
	ImGui::End();
}

App::~App()
{}


int App::Go()
{
	while( true )
	{
		// process all messages pending, but to not block for new messages
		if( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		const auto dt = timer.Mark() * speed_factor;
		HandleInput(dt);
		DoFrame(dt);
	}
}