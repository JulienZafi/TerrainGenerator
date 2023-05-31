#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

#include "Engine/Window.hpp"
#include "Application/App.hpp"

/*
* WINDOW DEFAULT PROPERTIES
*/
#define WIDTH	1280
#define HEIGHT	720
#define TITLE	"3D Terrain Generator"

/*
* MAP PROPERTIES
*/
#define MAP_DIM	256
glm::vec3 const MAP_CENTER { MAP_DIM / 2.0f, 0.0f, MAP_DIM / 2.0f };

int main()
{
	std::unique_ptr<Engine::Window> window{ std::make_unique<Engine::Window>(WIDTH, HEIGHT, TITLE) };
	window->SetViewPosition(MAP_CENTER);

	/*
	* RENDER LOOP
	*/
	std::unique_ptr <Application::Application> app{ std::make_unique<Application::Application>() };
	while (!window->ShouldClose())
	{
		window->UpdateTime();
		window->ProcessInput();

		// --------------------

		app->UpdateCameraPos(window);
		app->Render(window->ViewMatrix(), window->Zoom());

		// --------------------

		window->Wait();
	}

	window->Close();

	return 0;
}
