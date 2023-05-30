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
		Application() noexcept;
		~Application() = default;

		void Render(glm::mat4 const& viewMatrix, float const& cameraZoom) const noexcept;

		void UpdateCameraPos(std::unique_ptr <Engine::Window>& window) noexcept;

	private:
		std::unique_ptr <Engine::Terrain> m_terrain;

		Engine::Shader m_terrainShader;
	};
}