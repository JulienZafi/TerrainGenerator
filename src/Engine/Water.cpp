#include "Mesh.hpp"
#include "Water.hpp"

namespace Engine
{
	Water::Water(float const xPos, float const& zpos, unsigned int const width, unsigned int const height) noexcept
	{
		m_width = width;
		m_height = height;

		std::vector <Vertex> vertices{};
		for (unsigned int z{ 0 }; z < m_width - 1; ++z)
		{
			for (unsigned int x{ 0 }; x < m_height - 1; ++x)
			{
				Vertex vertex{};
				vertex.position.x = x;
				vertex.position.y = 0;

				//vertex.position.z = z - (float)height / 2.0f;
				vertex.position.z = z;

				vertex.textureCoords.x = static_cast<float>(x) / (float)m_width;
				vertex.textureCoords.y = static_cast<float>(z) / (float)m_height;
				vertices.push_back(vertex);
			}
		}

		std::vector <unsigned int> indices{};
		for (unsigned int i{ 0 }; i < m_width - 1; ++i)
		{
			for (unsigned int j{ 0 }; j < m_height - 1; ++j)
			{
				unsigned int topLeft{ i * m_width + j };
				unsigned int topRight{ topLeft + 1 };

				unsigned int bottomLeft{ (i + 1) * m_width + j };
				unsigned int bottomRight{ bottomLeft + 1 };

				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);

				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

		m_mesh = std::make_unique <Mesh>(vertices, indices);

		InitFrameBuffers();
	}

	void Water::InitFrameBuffers() noexcept
	{
		m_reflectionFrameBuffer = std::move(CreateFrameBuffer());
		m_reflectionTexture = std::move(CreateTexture(REFLECTION_WIDTH, REFLECTION_HEIGHT));
		m_reflectionDepthBuffer = std::move(CreateDepthBuffer(REFLECTION_WIDTH, REFLECTION_HEIGHT));
		UnbindCurrentFrameBuffer();

		m_refractionFrameBuffer = std::move(CreateFrameBuffer());
		m_refractionTexture = std::move(CreateTexture(REFRACTION_WIDTH, REFRACTION_HEIGHT));
		m_refractionDepthTexture = std::move(CreateDepthTexture(REFRACTION_WIDTH, REFRACTION_HEIGHT));
		UnbindCurrentFrameBuffer();
	}

	unsigned int Water::CreateFrameBuffer() noexcept
	{
		unsigned int frameBuffer{};
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		return frameBuffer;
	}

	unsigned int Water::CreateTexture(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int texture{};
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

		return texture;
	}

	unsigned int Water::CreateDepthTexture(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int texture{};
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

		return texture;
	}

	unsigned int Water::CreateDepthBuffer(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int depthBuffer{};
		glGenRenderbuffers(1, &depthBuffer);

		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

		return depthBuffer;
	}

	void Water::BindFrameBuffer(unsigned int const frameBuffer, unsigned int const width, unsigned int const height) const noexcept
	{
		glBindTexture(GL_TEXTURE_2D, 0); //To make sure the texture isn't bound
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		//glViewport(0, 0, width, height);
	}

	void Water::BindReflectionFrameBuffer() const noexcept
	{
		BindFrameBuffer(m_reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	}

	void Water::BindRefractionFrameBuffer() const noexcept
	{
		BindFrameBuffer(m_refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
	}

	void Water::UnbindCurrentFrameBuffer() const noexcept
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Render to screen top left
		//glViewport(0, 0, m_width, m_height);
	}

	void Water::Render(Shader const& terrainShader) const noexcept
	{
		glBindVertexArray(m_mesh->VAO());
		glDrawElements(GL_TRIANGLES, std::size(m_mesh->Indices()), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};