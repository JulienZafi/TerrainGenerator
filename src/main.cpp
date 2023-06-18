#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "Engine/Window.hpp"
#include "Application/App.hpp"
#include "Engine/Camera.hpp"

#include <imgui/imgui.h>


/*
* WINDOW DEFAULT PROPERTIES
*/
#define WIDTH	1920
#define HEIGHT	1280
#define TITLE	"3D Terrain Generator"

int main()
{
	std::unique_ptr<Engine::Window> window{ std::make_unique<Engine::Window>(WIDTH, HEIGHT, TITLE) };
	window->InitGUI();

	/*
	* RENDER LOOP
	*/
	std::unique_ptr <Application::Application> app{ std::make_unique<Application::Application>() };
	while (!window->ShouldClose())
	{
		window->UpdateTime();
		window->ProcessInput();
		window->startImGUIFrame();

		app->ShowGUI();

		// --------------------

		app->Render(window);

		window->DrawGUI();

		// --------------------

		window->Wait();
	}
	window->Close();

	return 0;
}
