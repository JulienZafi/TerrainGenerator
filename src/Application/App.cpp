
#include "App.hpp"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <iostream>

namespace Application
{
	Application::Application() noexcept
	{
		m_skyboxShader = Engine::Shader(SKYBOX_VSHADER_PATH, SKYBOX_FSHADER_PATH);
		m_skybox = std::make_unique <Engine::Skybox>();
		m_skyboxShader.UseProgram();
		m_skyboxShader.SetUniform("u_skyboxTexture", 0);

		m_terrainShader = Engine::Shader(TERRAIN_VSHADER_PATH, TERRAIN_FSHADER_PATH);
		m_terrain = std::make_unique <Engine::Terrain>();
		m_terrain->BindTextures(m_terrainShader);

		m_clearColor = { 0.1f, 0.1f, 0.1f };
		m_clipPlane = { 0.0f, 1.0f, 0.0f, 0.0f };
		m_cameraAltitude = 50.0f;
		m_zNear = 0.1f;
		m_zFar = 100000.0f;
		m_xpos = Engine::Camera::GetInstance()->Position().x;
		m_zpos = Engine::Camera::GetInstance()->Position().z;

		m_water = std::make_unique <Engine::Water>(m_xpos, m_zpos, m_terrain->Width(), m_terrain->Height());
		m_waterShader = Engine::Shader(WATER_VSHADER_PATH, WATER_FSHADER_PATH);
	}

	void Application::Render(std::unique_ptr <Engine::Window>& window) noexcept
	{
		glEnable(GL_CLIP_DISTANCE0);

		glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		* Update position
		*/
		m_xpos = Engine::Camera::GetInstance()->Position().x;
		m_zpos = Engine::Camera::GetInstance()->Position().z;
		m_terrain->UpdatePlayerPosition(m_xpos, m_zpos);

		/*
		* transformation matrix
		*/
		glm::mat4 projection = glm::perspective(glm::radians(Engine::Camera::GetInstance()->Zoom()),
			(float)m_terrain->Width() / (float)m_terrain->Height(),
			m_zNear,
			m_zFar);

		glm::mat4 view = Engine::Camera::GetInstance()->ViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		m_terrainShader.UseProgram();
		m_terrain->BindTextures(m_terrainShader);
		m_terrainShader.SetUniform<glm::mat4>("u_projection", projection);
		m_terrainShader.SetUniform<glm::mat4>("u_view", view);
		m_terrainShader.SetUniform<glm::mat4>("u_model", model);

		// RENDER TO REFLECTION FRAMEBUFFER
		glm::vec3 cameraPos = Engine::Camera::GetInstance()->Position();
		Engine::Camera::MirrorY(); // Mirror the camera on the water surface
		glm::mat4 reflectedView = Engine::Camera::GetInstance()->ViewMatrix(); // Recalculate the view matrix

		/*
		* RENDER TERRAIN TO REFLECTION BUFFER
		*/
		m_water->BindReflectionFrameBuffer();
		float cameraDistance{ 2 * (Engine::Camera::GetInstance()->Position().y) };
		Engine::Camera::Move(glm::vec3(0.0f, -cameraDistance, 0.0f));
		Engine::Camera::InvertPitch();
		m_terrainShader.UseProgram();
		m_terrainShader.SetUniform<glm::mat4>("u_view", reflectedView);
		m_terrainShader.SetUniform<glm::vec4>("u_clipPlane", m_clipPlane);

		m_terrain->Render(m_terrainShader);
		
		/*
		* RENDER TERRAIN TO REFRACTION BUFFER
		*/
		Engine::Camera::Move(glm::vec3(0.0f, cameraDistance, 0.0f));
		Engine::Camera::InvertPitch();

		m_water->BindRefractionFrameBuffer();
		m_terrainShader.UseProgram();
		m_terrainShader.SetUniform<glm::mat4>("u_view", reflectedView);
		m_clipPlane.z *= -1;
		m_terrainShader.SetUniform<glm::vec4>("u_clipPlane", m_clipPlane);

		m_terrain->Render(m_terrainShader);
		m_water->UnbindCurrentFrameBuffer();

		/* 
		* RENDER TERRAIN TO THE MAIN FRAMEBUFFER
		*/
		Engine::Camera::MirrorY(); // Reset camera position
		glDisable(GL_CLIP_DISTANCE0);
		m_terrainShader.UseProgram();
		m_terrainShader.SetUniform<glm::mat4>("u_view", view); // Use the original view matrix
		m_terrainShader.SetUniform<glm::vec4>("u_clipPlane", glm::vec4(0.0f, 1.0f, 0.0f, 10000.0f));

		m_terrain->Render(m_terrainShader);

		/*
		* RENDER WATER
		*/
		m_waterShader.UseProgram();
		m_waterShader.SetUniform<glm::mat4>("u_projection", projection);
		m_waterShader.SetUniform<glm::mat4>("u_view", view);
		m_waterShader.SetUniform<glm::mat4>("u_model", model);
		// Bind the reflection texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_water->ReflectionTexture());
		m_waterShader.SetUniform<int>("u_reflectionTexture", 0); // Pass texture unit 0 to the shader

		// Bind the refraction texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_water->RefractionTexture());
		m_waterShader.SetUniform<int>("u_refractionTexture", 1); // Pass texture unit 1 to the shader

		m_water->Render(m_waterShader);

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		m_skyboxShader.UseProgram();
		view = glm::mat4(glm::mat3(Engine::Camera::GetInstance()->ViewMatrix())); // remove translation from the view matrix
		m_skyboxShader.SetUniform("view", view);
		m_skyboxShader.SetUniform("projection", projection);
		m_skybox->Render();

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

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		m_terrain->ShowGUI();
	}
}