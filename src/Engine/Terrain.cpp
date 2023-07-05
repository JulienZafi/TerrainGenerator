#include "Terrain.hpp"
#include "Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <noise/noise.h>

#include <iostream>
#include <imgui/imgui.h>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>

namespace Engine
{
	Terrain::Terrain() noexcept
	{

		m_perlinParams.numOctaves = 8;
		m_perlinParams.persistence = 0.5f;
		m_perlinParams.multiNoiseNumOctaves = 6;
		m_perlinParams.frequency = 0.15f;
		m_perlinParams.lacunarity = 2.0f;
		m_perlinParams.scale = 0.05f;
		m_perlinParams.amplitude = 20.0f;

		m_numChunksToDisplay = NUM_CHUNCKS_TO_DISPLAY;
		m_chunkWidth = CHUNK_WIDTH;
		m_chunkHeight = CHUNK_HEIGHT;

		float initialX = (float)m_chunkWidth * (float)m_numChunksToDisplay / 2.0f;
		float initialZ = (float)m_chunkHeight * (float)m_numChunksToDisplay / 2.0f;

		UpdatePlayerPosition(initialX, initialZ);

		LoadTexture(TEXTURES_PATH + "grass.jpg", TextureType::GRASS);
		LoadTexture(TEXTURES_PATH + "rock.jpg", TextureType::ROCK);
		LoadTexture(TEXTURES_PATH + "sand.jpg", TextureType::SAND);
	}

	unsigned int Terrain::LoadTextureFromFile(std::string const& path) const noexcept
	{
		unsigned int textureID{};
		glGenTextures(1, &textureID);

		int width{}, height{}, numComponents{};
		unsigned char* data{ stbi_load(path.data(), &width, &height, &numComponents, 0) };
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
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

	void Terrain::LoadTexture(std::string const& fileName, TextureType const type) noexcept
	{
		unsigned int textureID{ LoadTextureFromFile(fileName) };
		m_textures[type] = textureID;
	}

	void Terrain::BindTextures(Shader const& shader) const noexcept
	{
		unsigned int textureUnit{ 0 };
		for (const auto& [type, texture] : m_textures)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, texture);
			
			switch (type)
			{
			case TextureType::GRASS:
				shader.SetUniform("u_textureGrass", textureUnit);
				break;
			case TextureType::ROCK:
				shader.SetUniform("u_textureRock", textureUnit);
				break;
			case TextureType::SAND:
				shader.SetUniform("u_textureSand", textureUnit);
				break;
			case TextureType::WATER:
				shader.SetUniform("u_textureWater", textureUnit);
				break;
			default:
				break;
			}
			textureUnit++;
		}
	}

	std::unique_ptr <Mesh> Terrain::ComputeVertices(float const& xPos, float const& zPos) noexcept
	{
		noise::module::Perlin perlinNoise{};
		perlinNoise.SetOctaveCount(m_perlinParams.numOctaves);

		perlinNoise.SetPersistence(m_perlinParams.persistence);

		noise::module::RidgedMulti perlinRidgedMultiNoise{};
		perlinRidgedMultiNoise.SetOctaveCount(m_perlinParams.multiNoiseNumOctaves);
		perlinRidgedMultiNoise.SetFrequency(m_perlinParams.frequency);
		perlinRidgedMultiNoise.SetLacunarity(m_perlinParams.lacunarity);

		std::vector <Vertex> vertices{};
		std::vector <unsigned int> indices{};
		vertices.reserve((m_chunkWidth + 1) * (m_chunkHeight + 1) * 3);
		indices.reserve(m_chunkWidth * m_chunkHeight * 6);

		float scale{ m_perlinParams.scale };
		float amplitude{ m_perlinParams.amplitude };

		for (unsigned int z{ 0 }; z < m_chunkHeight + 1; ++z)
		{
			for (unsigned int x{ 0 }; x < m_chunkWidth + 1; ++x)
			{
				Vertex vertex{};
				vertex.position.x = xPos + static_cast<float>(x);
				vertex.position.z = zPos + static_cast<float>(z);
				
				vertex.textureCoords.x = static_cast<float>(x) / (float)m_chunkWidth;
				vertex.textureCoords.y = static_cast<float>(z) / (float)m_chunkHeight;

				float yPos{ static_cast<float>(perlinRidgedMultiNoise.GetValue(vertex.position.x * scale, 0.0, vertex.position.z * scale)) * amplitude };

				vertex.position.y = yPos;

				vertices.push_back(vertex);
			}
		}

		for (unsigned int i{ 0 }; i < m_chunkHeight; ++i)
		{
			for (unsigned int j{ 0 }; j < m_chunkWidth; ++j)
			{
				unsigned int topLeft{ i * (m_chunkWidth + 1) + j };
				unsigned int topRight{ topLeft + 1 };

				unsigned int bottomLeft{ (i + 1) * (m_chunkWidth + 1) + j };
				unsigned int bottomRight{ bottomLeft + 1 };

				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);

				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

		for (unsigned int i{ 0 }; i < m_chunkHeight; ++i)
		{
			for (unsigned int j{ 0 }; j < m_chunkWidth; ++j)
			{
				unsigned int topLeft = i * (m_chunkWidth + 1) + j;
				unsigned int topRight = topLeft + 1;
				unsigned int bottomLeft = (i + 1) * (m_chunkWidth + 1) + j;
				unsigned int bottomRight = bottomLeft + 1;

				glm::vec3 v0 = vertices[topLeft].position;
				glm::vec3 v1 = vertices[bottomLeft].position;
				glm::vec3 v2 = vertices[topRight].position;
				glm::vec3 faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

				vertices[topLeft].normal += faceNormal;
				vertices[bottomLeft].normal += faceNormal;
				vertices[topRight].normal += faceNormal;

				v0 = vertices[topRight].position;
				v1 = vertices[bottomLeft].position;
				v2 = vertices[bottomRight].position;
				faceNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

				vertices[topRight].normal += faceNormal;
				vertices[bottomLeft].normal += faceNormal;
				vertices[bottomRight].normal += faceNormal;
			}
		}

		for (auto& vertex : vertices)
		{
			vertex.normal = glm::normalize(vertex.normal);
		}

		return std::make_unique <Mesh>(vertices, indices);
	}

	void Terrain::Update(float const& x, float const& z) noexcept
	{
		/*
		* Current chunk coordinates
		*/
		int currentChunkX{ (int)round(x / (float)m_chunkWidth) };
		int currentChunkZ{ (int)round(z / (float)m_chunkHeight) };

		/*
		* Delete chunks outside visualization distance
		*/
		for (auto it{ std::begin(m_meshes) }; it != std::end(m_meshes);)
		{
			int dx{ abs(it->first.first - currentChunkX) };
			int dz{ abs(it->first.second - currentChunkZ) };

			if (dx > m_numChunksToDisplay || dz > m_numChunksToDisplay)
			{
				it = m_meshes.erase(it);
			}
			else
			{
				++it;
			}
		}

		/*
		* Load chunks inside visualization distance
		*/
		for (int x{ currentChunkX - m_numChunksToDisplay }; x < currentChunkX + m_numChunksToDisplay; ++x)
		{
			for (int z{ currentChunkZ - m_numChunksToDisplay }; z < currentChunkZ + m_numChunksToDisplay; ++z)
			{
				auto chunkKey{ std::make_pair(x, z) };

				if (m_meshes.find(chunkKey) == m_meshes.end())
				{
					int xPos{ x * m_chunkWidth };
					int zPos{ z * m_chunkHeight };
					m_meshes[chunkKey] = std::move(ComputeVertices(xPos, zPos));
				}
				else{}
			}
		}
	}

	void Terrain::UpdatePlayerPosition(float const& x, float const& z) noexcept
	{
		/*
		* Calculate the chunk where the player is
		*/
		int currentChunkX = (int)round(x / (float)m_chunkWidth);
		int currentChunkZ = (int)round(z / (float)m_chunkHeight);

		/*
		* Generate missing chunks around player
		*/
		for (int dx = -m_numChunksToDisplay; dx <= m_numChunksToDisplay; ++dx)
		{
			for (int dz = -m_numChunksToDisplay; dz <= m_numChunksToDisplay; ++dz)
			{
				int chunkX = currentChunkX + dx;
				int chunkZ = currentChunkZ + dz;

				std::pair<int, int> chunkKey = std::make_pair(chunkX, chunkZ);
				if (m_meshes.find(chunkKey) == m_meshes.end())
				{
					int xPos = chunkX * m_chunkWidth;
					int zPos = chunkZ * m_chunkHeight;
					m_meshes[chunkKey] = std::move(ComputeVertices(xPos, zPos));
				}
				else{}
			}
		}

		Update(x, z);
	}

	void Terrain::Render(Shader const& shader, glm::mat4 const& projection, glm::mat4 const& view, glm::mat4 const& model) const noexcept
	{
		shader.UseProgram();
		BindTextures(shader);
		shader.SetUniform("u_projection", projection);
		shader.SetUniform("u_view", view);
		shader.SetUniform("u_model", model);
		shader.SetUniform("u_clipPlane", m_clipPlane);
		shader.SetUniform("u_light.direction", m_lightDirection);
		shader.SetUniform("u_light.color", m_lightColor);

		for (const auto& [coord, mesh] : m_meshes)
		{	
			glBindVertexArray(mesh->VAO());
			glDrawElements(GL_TRIANGLES, std::size(mesh->Indices()), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}

	void Terrain::SetLight(glm::vec3 const& lightColor, glm::vec3 const &lightDirection) noexcept
	{
		m_lightColor = lightColor;
		m_lightDirection = lightDirection;
	}

	void Terrain::SetClipPlane(glm::vec4 const& clipPlane) noexcept
	{
		m_clipPlane = clipPlane;
	}

	void Terrain::ShowGUI() noexcept
	{
		ImGui::Begin("Terrain settings : ");
		ImGui::DragInt("Num chunks to display", &m_numChunksToDisplay, 1, 1, 10);
		ImGui::DragInt("Chunk width", &m_chunkWidth, 1, 10, 256);
		ImGui::DragInt("Chunk height", &m_chunkHeight, 1, 10, 256);
		ImGui::End();

		ImGui::Begin("Perlin Noise settings : ");
		ImGui::DragInt("Num octaves", &m_perlinParams.numOctaves, 1, 1, 50);
		ImGui::DragFloat("Persistence", &m_perlinParams.persistence, 0.05, 0.1, 10.0);

		ImGui::DragInt("Perlin Ridged num octaves", &m_perlinParams.multiNoiseNumOctaves, 1, 1, 50);
		ImGui::DragFloat("Frequency", &m_perlinParams.frequency, 0.05, 0.1, 50.0);
		ImGui::DragFloat("Lacunarity", &m_perlinParams.lacunarity, 0.05, 0.1, 50.0);

		ImGui::DragFloat("Scale", &m_perlinParams.scale, 0.05, 0.1, 50.0);
		ImGui::DragFloat("Amplitude", &m_perlinParams.amplitude, 1.0, 0.0, 200.0);
		ImGui::End();
	}
}