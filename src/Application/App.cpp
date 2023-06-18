
#include "App.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

namespace Application
{
	Application::Application() noexcept
	{
		m_terrain = std::make_unique <Engine::Terrain>();

		/*
		* CREATE GPU PROGRAM
		*/
		m_terrainShader = Engine::Shader(TERRAIN_VSHADER_PATH, TERRAIN_FSHADER_PATH);

		m_clearColor = { 0.1f, 0.1f, 0.1f };
		m_cameraAltitude = 50.0f;
		m_zNear = 0.1f;
		m_zFar = 100000.0f;
		m_xpos = Engine::Camera::GetInstance()->Position().x;
		m_zpos = Engine::Camera::GetInstance()->Position().z;
	}

	void Application::Render(std::unique_ptr <Engine::Window>& window) noexcept
	{
		glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		* Update position
		*/
		m_xpos = Engine::Camera::GetInstance()->Position().x;
		m_zpos = Engine::Camera::GetInstance()->Position().z;
		m_terrain->UpdatePlayerPosition(m_xpos, m_zpos);

		m_terrainShader.UseProgram();

		/*
		* view / projection transformations
		*/
		glm::mat4 projection = glm::perspective(glm::radians(Engine::Camera::GetInstance()->Zoom()),
												(float)m_terrain->Width() / (float)m_terrain->Height(), 
												m_zNear,
												m_zFar);

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
		Engine::Camera::UpdateHeight(m_cameraAltitude);
	}

	void Application::ShowGUI()
	{
		ImGui::Begin("General settings : ");
		ImGui::Text("Clear color :");
		ImGui::DragFloat3("", &m_clearColor[0], 0.05f, 0.0f, 1.0f);

		ImGui::Text("Current player position :");
		ImGui::Text("x : %f", m_xpos);
		ImGui::Text("y : %f", m_zpos);

		ImGui::Text("Projection matrix control :");
		ImGui::DragFloat("zNear", &m_zNear, 0.1f, 0.1f, 100.0f);
		ImGui::DragFloat("zFar", &m_zFar, 100.0f, 10000.0f, 100000.0f);

		ImGui::Text("Camera altitude :");
		ImGui::DragFloat(" ", &m_cameraAltitude, 1.0f, 0.0f, 500.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		m_terrain->ShowGUI();
	}
}