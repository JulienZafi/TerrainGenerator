#include "Camera.hpp"
#include "Mesh.hpp"
#include "Water.hpp"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <stb_image.h>

#include <iostream>

namespace Engine
{
	Water::Water(float const xpos, float const& zpos, unsigned int const width, unsigned int const height, float const& windowWidth, float const& windowHeight) noexcept
	{
		m_xpos = xpos;
		m_zpos = zpos;
		m_width = width;
		m_height = height;
		m_textureWidth = windowWidth;
		m_textureHeight = windowHeight;

		m_waveLevel = 0.03f;
		m_waveSpeed = 0.05;
		m_moveFactor = 0.0f;
		m_tiling = 10.0f;
		m_waterColor = { 0.0f, 0.3f, 0.5f };
		m_refractiveFactor = 0.5f;
		m_shineDamper = 20.0f;
		m_reflectivity = 0.6f;

		std::vector<Vertex> vertices(m_width * m_height);
		int initialx = (int)round(m_xpos - (float)m_width / 2.0f) + OFFSET_POS;
		int initialz = (int)round(m_zpos - (float)m_height / 2.0f) + OFFSET_POS;
		for (int z = 0; z < m_height; ++z)
		{
			for (int x = 0; x < m_width; ++x)
			{
				Vertex& vertex = vertices[z * m_width + x];
				vertex.position = { initialx + x, 0, initialz + z};
				vertex.textureCoords = {static_cast<float>(x) / (m_width - 1), static_cast<float>(z) / (m_height - 1)};
			}
		}

		std::vector<unsigned int> indices(6 * (m_width - 1) * (m_height - 1));

		for (unsigned int z = 0; z < m_height - 1; ++z)
		{
			for (unsigned int x = 0; x < m_width - 1; ++x)
			{
				unsigned int index = 6 * (z * (m_width - 1) + x);

				unsigned int topLeft = (z * m_width) + x;
				unsigned int topRight = topLeft + 1;
				unsigned int bottomLeft = ((z + 1) * m_width) + x;
				unsigned int bottomRight = bottomLeft + 1;

				// Triangle 1
				indices[index] = topLeft;
				indices[index + 1] = bottomLeft;
				indices[index + 2] = topRight;

				// Triangle 2
				indices[index + 3] = topRight;
				indices[index + 4] = bottomLeft;
				indices[index + 5] = bottomRight;

				// Calculate face normal
				glm::vec3 v0 = vertices[topLeft].position;
				glm::vec3 v1 = vertices[bottomLeft].position;
				glm::vec3 v2 = vertices[topRight].position;
				glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

				// Accumulate face normal to the vertex normals
				vertices[topLeft].normal += faceNormal;
				vertices[bottomLeft].normal += faceNormal;
				vertices[topRight].normal += faceNormal;
			}
		}

		// Normalize vertex normals
		for (auto& vertex : vertices)
		{
			vertex.normal = glm::normalize(vertex.normal);
		}

		m_mesh = std::make_unique <Mesh>(vertices, indices);

		InitFrameBuffers();

		m_dudvMap = LoadTextureFromFile(DUDV_FILE);
		m_normalMap = LoadTextureFromFile(NORMAL_FILE);
	}

	void Water::UpdateMesh(glm::vec3 const &camPosition) noexcept
	{
		float dx{ camPosition.x - m_xpos };
		float dz{ camPosition.z - m_zpos };

		if (dx * dz > 250.0f)
		{
			std::vector<Vertex> vertices(m_width * m_height);
			int initialx = (int)round(camPosition.x - (float)m_width / 2.0f) + OFFSET_POS;
			int initialz = (int)round(camPosition.z - (float)m_height / 2.0f) + OFFSET_POS;
			for (int z = 0; z < m_height; ++z)
			{
				for (int x = 0; x < m_width; ++x)
				{
					Vertex& vertex = vertices[z * m_width + x];
					vertex.position = { initialx + x, 0, initialz + z };
					vertex.textureCoords = { static_cast<float>(x) / (m_width - 1), static_cast<float>(z) / (m_height - 1) };
				}
			}

			std::vector<unsigned int> indices(6 * (m_width - 1) * (m_height - 1));

			for (unsigned int z = 0; z < m_height - 1; ++z)
			{
				for (unsigned int x = 0; x < m_width - 1; ++x)
				{
					unsigned int index = 6 * (z * (m_width - 1) + x);

					unsigned int topLeft = (z * m_width) + x;
					unsigned int topRight = topLeft + 1;
					unsigned int bottomLeft = ((z + 1) * m_width) + x;
					unsigned int bottomRight = bottomLeft + 1;

					// Triangle 1
					indices[index] = topLeft;
					indices[index + 1] = bottomLeft;
					indices[index + 2] = topRight;

					// Triangle 2
					indices[index + 3] = topRight;
					indices[index + 4] = bottomLeft;
					indices[index + 5] = bottomRight;

					// Calculate face normal
					glm::vec3 v0 = vertices[topLeft].position;
					glm::vec3 v1 = vertices[bottomLeft].position;
					glm::vec3 v2 = vertices[topRight].position;
					glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

					// Accumulate face normal to the vertex normals
					vertices[topLeft].normal += faceNormal;
					vertices[bottomLeft].normal += faceNormal;
					vertices[topRight].normal += faceNormal;
				}
			}

			// Normalize vertex normals
			for (auto& vertex : vertices)
			{
				vertex.normal = glm::normalize(vertex.normal);
			}

			m_mesh = std::make_unique <Mesh>(vertices, indices);

			m_xpos = camPosition.x;
			m_zpos = camPosition.z;
		}
		else{}
	}

	void Water::InitFrameBuffers() noexcept
	{
		m_reflectionFrameBuffer = CreateFrameBuffer();
		m_reflectionTexture = CreateTexture(m_textureWidth, m_textureHeight);
		m_reflectionDepthBuffer = CreateDepthTexture(m_textureWidth, m_textureHeight);
		UnbindCurrentFrameBuffer();
		m_refractionFrameBuffer = CreateFrameBuffer();
		m_refractionTexture = CreateTexture(m_textureWidth, m_textureHeight);
		m_refractionDepthTexture = CreateDepthTexture(m_textureWidth, m_textureHeight);
		UnbindCurrentFrameBuffer();
	}

	unsigned int Water::CreateFrameBuffer() noexcept
	{
		unsigned int frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		return frameBuffer;
	}

	unsigned int Water::CreateTexture(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		return texture;
	}

	unsigned int Water::CreateDepthTexture(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

		return texture;
	}

	void Water::BindFrameBuffer(unsigned int const frameBuffer, unsigned int const width, unsigned int const height) const noexcept
	{
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	}

	unsigned int Water::LoadTextureFromFile(std::string_view const& path) const noexcept
	{
		unsigned int textureID{};
		glGenTextures(1, &textureID);

		int width{}, height{}, numComponents{};
		unsigned char* data{ stbi_load(path.data(), &width, &height, &numComponents, 0)};
		if (data)
		{
			GLenum format{};
			if (numComponents == 1)
			{
				format = GL_RED;
			}
			else if (numComponents == 3)
			{
				format = GL_RGB;
			}
			else if (numComponents == 4)
			{
				format = GL_RGBA;
			}
			else {}

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);

			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(textureID);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load : " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}

	void Water::BindReflectionFrameBuffer() const noexcept
	{
		BindFrameBuffer(m_reflectionFrameBuffer, m_textureWidth, m_textureHeight);
	}

	void Water::BindRefractionFrameBuffer() const noexcept
	{
		BindFrameBuffer(m_refractionFrameBuffer, m_textureWidth, m_textureHeight);
	}

	void Water::UnbindCurrentFrameBuffer() const noexcept
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Water::SetLightProperties(glm::vec3 const& lightDirection, glm::vec3 const& lightColor) noexcept
	{
		m_lightDirection = lightDirection;
		m_lightColor = lightColor;
	}

	void Water::SetTime(float const& deltaTime) noexcept
	{
		m_deltaTime = deltaTime;
	}

	void Water::Render(Shader const& shader, glm::mat4 const& projection, glm::mat4 const& view, glm::mat4 const& model) noexcept
	{
		m_moveFactor += m_waveSpeed * m_deltaTime;

		if (m_moveFactor > 1.0f)
		{
			m_moveFactor = 0.0f;
		}

		shader.UseProgram();
		shader.SetUniform("u_projection", projection);
		shader.SetUniform("u_view", view);
		shader.SetUniform("u_model", model);

		shader.SetUniform("u_cameraPosition", Camera::GetInstance()->Position());
		shader.SetUniform("u_lightColor", m_lightColor);
		shader.SetUniform("u_lightDirection", m_lightDirection);

		shader.SetUniform("u_waterColor", m_waterColor);
		shader.SetUniform("u_waveLevel", m_waveLevel);
		shader.SetUniform("u_moveFactor", m_moveFactor);
		shader.SetUniform("u_tiling", m_tiling);
		shader.SetUniform("u_refractiveFactor", m_refractiveFactor);
		shader.SetUniform("u_shineDamper", m_shineDamper);
		shader.SetUniform("u_reflectivity", m_reflectivity);

		// Bind the reflection texture to texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_reflectionTexture);
		shader.SetUniform("u_reflectionTexture", 0); // Pass texture unit 0 to the shader

		// Bind the refraction texture to texture unit 1
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_refractionTexture);
		shader.SetUniform("u_refractionTexture", 1); // Pass texture unit 1 to the shader

		// Bind DUDV map
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_dudvMap);
		shader.SetUniform("u_dudvMap", 2); // Pass texture unit 1 to the shader

		// Bind normal map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_normalMap);
		shader.SetUniform("u_normalMap", 3); // Pass texture unit 1 to the shader		

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindVertexArray(m_mesh->VAO());
		glDrawElements(GL_TRIANGLES, std::size(m_mesh->Indices()), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
	}

	void Water::ShowGUI() noexcept
	{
		ImGui::Begin("Water settings : ");
		ImGui::DragFloat4("water color", &m_waterColor[0], 0.01f, 0.0f, 1.0f);
		ImGui::DragFloat("wave level", &m_waveLevel, 0.001f, 0.01f, 1.0f);
		ImGui::DragFloat("wave speed", &m_waveSpeed, 0.005f, 0.0f, 50.0f);
		ImGui::DragFloat("move factor", &m_moveFactor, 0.005f, 0.1f, 1.5f);
		ImGui::DragFloat("tiling", &m_tiling, 0.1f, 0.1f, 20.0f);
		ImGui::DragFloat("refractive factor", &m_refractiveFactor, 0.1f, 0.0f, 20.0f);
		ImGui::DragFloat("shine damper", &m_shineDamper, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("reflectivity", &m_reflectivity, 0.01f, 0.0f, 1.0f);
		ImGui::End();
	}
};