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
	constexpr std::string_view const TERRAIN_VSHADER_PATH{ "../../shaders/terrain/terrain.vs" };
	constexpr std::string_view const TERRAIN_FSHADER_PATH{ "../../shaders/terrain/terrain.fs" };

	class Application
	{
	public:
		Application() = delete;
		~Application() = default;
		Application(unsigned int const width, unsigned int const height) noexcept;

		void Render(unsigned int const width, unsigned int const height, glm::mat4 const& viewMatrix, float const& cameraZoom) const noexcept;

		void UpdateCameraPos(std::unique_ptr <Engine::Window>& window) noexcept;

	private:
		unsigned int m_width, m_height;
		std::unique_ptr <Engine::Terrain> m_terrain;

		Engine::Shader m_terrainShader;
	};
}