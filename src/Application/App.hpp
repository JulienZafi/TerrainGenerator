#pragma once

#include "Engine/Shader.hpp"
#include "Engine/Skybox.hpp"
#include "Engine/Terrain.hpp"
#include "Engine/Window.hpp"
#include "Engine/Water.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <string_view>
#include <vector>

namespace Application
{
	constexpr std::string_view const TERRAIN_VSHADER_PATH{ "shaders//terrain//terrain.vs" };
	constexpr std::string_view const TERRAIN_FSHADER_PATH{ "shaders//terrain//terrain.fs" };
	constexpr std::string_view const WATER_VSHADER_PATH{ "shaders//water//water.vs" };
	constexpr std::string_view const WATER_FSHADER_PATH{ "shaders//water//water.fs" };
	constexpr std::string_view const SKYBOX_VSHADER_PATH{ "shaders//skybox//skybox.vs" };
	constexpr std::string_view const SKYBOX_FSHADER_PATH{ "shaders//skybox//skybox.fs" };

	class Application
	{
	public:
		Application() noexcept;
		~Application() = default;

		void Render(std::unique_ptr <Engine::Window>& window) noexcept;

		void ShowGUI();

	private:
		std::unique_ptr <Engine::Skybox> m_skybox;
		std::unique_ptr <Engine::Terrain> m_terrain;
		std::unique_ptr <Engine::Water> m_water;

		Engine::Shader m_skyboxShader;
		Engine::Shader m_terrainShader;
		Engine::Shader m_waterShader;

		glm::vec3 m_clearColor;
		glm::vec4 m_clipPlane;
		float m_cameraAltitude;
		float m_zNear;
		float m_zFar;
		float m_xpos;
		float m_zpos;

		glm::vec3 m_waterColor;
		float m_waterWaveLength;
		float m_waveFactor;
		float m_waveSpeed;
		float m_reflectiveFactor;
		glm::vec3 m_lightColor;
		glm::vec3 m_lightPosition;
		glm::vec3 m_lightDirection;
	};
}