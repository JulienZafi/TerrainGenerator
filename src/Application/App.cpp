
#include "App.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <iostream>

namespace Application
{
	Application::Application(float const& windowWidth, float const& windowHeight) noexcept
	{
		m_terrainShader = Engine::Shader(TERRAIN_VSHADER_PATH, TERRAIN_FSHADER_PATH);
		m_terrain = std::make_unique <Engine::Terrain>();
		m_terrainShader.UseProgram();
		m_terrain->BindTextures(m_terrainShader);

		m_clearColor = { 0.1f, 0.1f, 0.1f };
		m_clipPlane = { 0.0f, 1.0f, 0.0f, -0.1f };
		m_cameraAltitude = 50.0f;
		m_zFar = 10000;
		m_zNear = 0.1f;

		m_xpos = Engine::Camera::GetInstance()->Position().x;
		m_zpos = Engine::Camera::GetInstance()->Position().z;

		m_lightColor = {0.7f, 0.7f, 0.7f};
		m_lightDirection = { -0.2f, -1.0f, -0.3f };

		m_water = std::make_unique <Engine::Water>(m_xpos, m_zpos, 2 * m_terrain->Width() + 50, 2 * m_terrain->Height() + 50, windowWidth, windowHeight);
		m_waterShader = Engine::Shader(WATER_VSHADER_PATH, WATER_FSHADER_PATH);

		m_skyboxShader = Engine::Shader(SKYBOX_VSHADER_PATH, SKYBOX_FSHADER_PATH);
		m_skybox = std::make_unique <Engine::Skybox>();
		m_skyboxShader.UseProgram();
		m_skyboxShader.SetUniform("u_skyboxTexture", 0);
	}

	void Application::Render(std::unique_ptr <Engine::Window> const& window)
	{
		glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		* Update position
		*/
		m_xpos = Engine::Camera::GetInstance()->Position().x;
		m_zpos = Engine::Camera::GetInstance()->Position().z;
		m_terrain->UpdatePlayerPosition(m_xpos, m_zpos);
		m_water->UpdateMesh(Engine::Camera::GetInstance()->Position());

		/*
		* transformation matrix
		*/
		glm::mat4 projection = glm::perspective(glm::radians(Engine::Camera::GetInstance()->Zoom()),
			(float)m_terrain->Width() / (float)m_terrain->Height(),
			m_zNear,
			m_zFar);
		glm::mat4 view = Engine::Camera::GetInstance()->ViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		// RENDER TO REFLECTION FRAMEBUFFER
		m_water->BindReflectionFrameBuffer();
		glEnable(GL_CLIP_DISTANCE0);
		glDisable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		float cameraDistance{ 2 * (Engine::Camera::GetInstance()->Position().y) };
		Engine::Camera::Move(glm::vec3(0.0f, -cameraDistance, 0.0f));
		Engine::Camera::InvertPitch();
		glm::mat4 reflectedView = Engine::Camera::GetInstance()->ViewMatrix(); // Recalculate the view matrix

		m_terrain->SetLight(m_lightColor, m_lightDirection);
		m_terrain->SetClipPlane(m_clipPlane);
		m_terrain->Render(m_terrainShader, projection, reflectedView, model);

		view = glm::mat4(glm::mat3(Engine::Camera::GetInstance()->ViewMatrix())); // remove translation from the view matrix
		m_skybox->Render(m_skyboxShader, projection, view);

		m_water->UnbindCurrentFrameBuffer();
		
		/*
		* RENDER TERRAIN TO REFRACTION BUFFER
		*/
		Engine::Camera::Move(glm::vec3(0.0f, cameraDistance, 0.0f));
		Engine::Camera::InvertPitch();
		reflectedView = Engine::Camera::GetInstance()->ViewMatrix(); // Recalculate the view matrix

		m_water->BindRefractionFrameBuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec4 refractionClipPlane{ -m_clipPlane };

		m_terrain->SetLight(m_lightColor, m_lightDirection);
		m_terrain->SetClipPlane(refractionClipPlane);
		m_terrain->Render(m_terrainShader, projection, reflectedView, model);

		view = glm::mat4(glm::mat3(Engine::Camera::GetInstance()->ViewMatrix())); // remove translation from the view matrix
		m_skybox->Render(m_skyboxShader, projection, view);

		m_water->UnbindCurrentFrameBuffer();

		/* 
		* RENDER TERRAIN TO THE MAIN FRAMEBUFFER
		*/
		glDisable(GL_CLIP_DISTANCE0);
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_terrain->SetLight(m_lightColor, m_lightDirection);
		m_terrain->SetClipPlane(glm::vec4(0.0f, 1.0f, 0.0f, 10000.0f));
		m_terrain->Render(m_terrainShader, projection, reflectedView, model);

		m_water->SetLightProperties(m_lightDirection, m_lightColor);
		m_water->SetTime(window->DeltaTime());
		m_water->Render(m_waterShader, projection, reflectedView, model);

		view = glm::mat4(glm::mat3(Engine::Camera::GetInstance()->ViewMatrix())); // remove translation from the view matrix
		m_skybox->Render(m_skyboxShader, projection, view);

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
		ImGui::Text("Reflection Clipping plane position :");
		ImGui::DragFloat4(" ", &m_clipPlane[0], 1.0f, -500.0f, 500.0f);
		ImGui::Text("Light properties :");
		ImGui::DragFloat3("light direction", &m_lightDirection[0], 0.0f, 0.0f, 100000.0f);
		ImGui::DragFloat3("light colour", &m_lightColor[0], 0.0f, 0.0f, 1.0f);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		m_terrain->ShowGUI();

		m_water->ShowGUI();
	}
}