#include "Mesh.hpp"
#include "Water.hpp"

#include <stb_image.h>

#include <iostream>

namespace Engine
{
	Water::Water(float const xPos, float const& zpos, unsigned int const width, unsigned int const height) noexcept
	{
		m_width = width;
		m_height = height;

	std::vector<Vertex> vertices(m_width * m_height);

	for (unsigned int z = 0; z < m_height; ++z)
	{
		for (unsigned int x = 0; x < m_width; ++x)
		{
			Vertex& vertex = vertices[z * m_width + x];
			vertex.position = {xPos + x, 0, zpos + z};
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
		}
	}

		m_mesh = std::make_unique <Mesh>(vertices, indices);

		InitFrameBuffers();

		m_dudvMap = LoadTextureFromFile(DUDV_FILE);
	}

	void Water::InitFrameBuffers() noexcept
	{
		m_reflectionFrameBuffer = CreateFrameBuffer();
		m_reflectionTexture = CreateTexture(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		m_reflectionDepthBuffer = CreateDepthBuffer(REFLECTION_WIDTH, REFLECTION_HEIGHT);
		UnbindCurrentFrameBuffer();
		m_refractionFrameBuffer = CreateFrameBuffer();
		m_refractionTexture = CreateTexture(REFRACTION_WIDTH, REFRACTION_HEIGHT);
		m_refractionDepthTexture = CreateDepthTexture(REFRACTION_WIDTH, REFRACTION_HEIGHT);
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
		unsigned int texture{};
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		return texture;
	}

	unsigned int Water::CreateDepthTexture(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);

		return texture;
	}

	unsigned int Water::CreateDepthBuffer(unsigned int const width, unsigned int const height) noexcept
	{
		unsigned int depthBuffer;
		glGenRenderbuffers(1, &depthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
		return depthBuffer;
	}

	unsigned int Water::CreateRenderBufferAttachment(int width, int height) noexcept
	{
		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it

		return rbo;
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
		BindFrameBuffer(m_reflectionFrameBuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
	}

	void Water::BindRefractionFrameBuffer() const noexcept
	{
		BindFrameBuffer(m_refractionFrameBuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
	}

	void Water::UnbindCurrentFrameBuffer() const noexcept
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Water::Render(Shader const& terrainShader) const noexcept
	{
		glBindVertexArray(m_mesh->VAO());
		glDrawElements(GL_TRIANGLES, std::size(m_mesh->Indices()), GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};