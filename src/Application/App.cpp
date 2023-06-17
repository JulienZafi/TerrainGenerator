
#include "App.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Application
{
	Application::Application() noexcept
	{
		m_terrain = std::make_unique <Engine::Terrain>();

		/*
		* CREATE GPU PROGRAM
		*/
		m_terrainShader = Engine::Shader(TERRAIN_VSHADER_PATH, TERRAIN_FSHADER_PATH);
	}

	void Application::Render(std::unique_ptr <Engine::Window>& window) noexcept
	{
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		* Update position
		*/
		m_terrain->UpdatePlayerPosition(Engine::Camera::GetInstance()->Position().x, Engine::Camera::GetInstance()->Position().z);

		m_terrainShader.UseProgram();

		/*
		* view / projection transformations
		*/
		glm::mat4 projection = glm::perspective(glm::radians(Engine::Camera::GetInstance()->Zoom()),
												(float)m_terrain->Width() / (float)m_terrain->Height(), 
												0.1f, 
												100000.0f);
		glm::mat4 view = Engine::Camera::GetInstance()->ViewMatrix();
		m_terrainShader.SetUniform<glm::mat4>("u_projection", projection);
		m_terrainShader.SetUniform<glm::mat4>("u_view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		m_terrainShader.SetUniform<glm::mat4>("u_model", model);

		/*
		* RENDER
		*/
		m_terrain->RenderChunks(m_terrainShader);

		/*
		* Update Camera height
		*/
		//float height{ m_terrain->GetHeightAtPos(Engine::Camera::GetInstance()->Position().x, Engine::Camera::GetInstance()->Position().z) };
		float height{ 50.0f };
		Engine::Camera::UpdateHeight(height);
	}
}