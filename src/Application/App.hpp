#pragma once

#include "Engine/Shader.hpp"
#include "Engine/Terrain.hpp"
#include "Engine/Window.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string_view>
#include <vector>

namespace Application
{
	constexpr std::string_view const TERRAIN_VSHADER_PATH{ "shaders/terrain/terrain.vs" };
	constexpr std::string_view const TERRAIN_FSHADER_PATH{ "shaders/terrain/terrain.fs" };

	class Application
	{
	public:
		Application() noexcept;
		~Application() = default;

		void Render(std::unique_ptr <Engine::Window>& window) noexcept;

		void ShowGUI();

	private:
		std::unique_ptr <Engine::Terrain> m_terrain;

		Engine::Shader m_terrainShader;

		glm::vec3 m_clearColor;
		float m_cameraAltitude;
		float m_zNear;
		float m_zFar;
		float m_xpos;
		float m_zpos;
	};
}