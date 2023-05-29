
#include "App.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

namespace Application
{
	Application::Application(unsigned int const width, unsigned int const height) noexcept
	{
		m_width = width;
		m_height = height;
		m_terrain = std::make_unique <Engine::Terrain>(m_width, m_height);

		/*
		* CREATE GPU PROGRAM
		*/
		m_terrainShader = Engine::Shader(TERRAIN_VSHADER_PATH, TERRAIN_FSHADER_PATH);
	}

	void Application::Render(unsigned int const width, unsigned int const height, glm::mat4 const& viewMatrix, float const& cameraZoom) const noexcept
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_terrainShader.UseProgram();

		/*
		* view / projection transformations
		*/
		glm::mat4 projection = glm::perspective(glm::radians(cameraZoom), (float)width / (float)height, 0.1f, 100000.0f);
		glm::mat4 view = viewMatrix;
		m_terrainShader.SetUniform<glm::mat4>("u_projection", projection);
		m_terrainShader.SetUniform<glm::mat4>("u_view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		m_terrainShader.SetUniform<glm::mat4>("u_model", model);
		
		/*
		* RENDER
		*/
		m_terrain->RenderChunks(m_terrainShader);
	}

	void Application::UpdateCameraPos(std::unique_ptr <Engine::Window>& window) noexcept
	{
		glm::vec3 cameraPos{ window->CameraPosition() };

		float height{ m_terrain->GetHeightAtPos(cameraPos.x, cameraPos.z) };

		window->UpdateCameraPos(height);

		m_terrain->UpdatePlayerPosition(cameraPos.x, cameraPos.z);
	}
}