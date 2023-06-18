#include "Terrain.hpp"
#include "Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <imgui/imgui.h>

namespace Engine
{
	Terrain::Terrain() noexcept
	{
		m_perlinParams.numOctaves = 8;
		m_perlinParams.persistence = 0.5f;
		m_perlinParams.multiNoiseNumOctaves = 6;
		m_perlinParams.frequency = 0.1f;
		m_perlinParams.lacunarity = 2.0f;
		m_perlinParams.scale = 0.05f;
		m_perlinParams.amplitude = 20.0f;

		m_numChunksToDisplay = NUM_CHUNCKS_TO_DISPLAY;
		m_chunkWidth = CHUNK_WIDTH;
		m_chunkHeight = CHUNK_HEIGHT;

		float initialX = (float)m_chunkWidth * (float)m_numChunksToDisplay / 2.0f;
		float initialZ = (float)m_chunkHeight * (float)m_numChunksToDisplay / 2.0f;

		UpdatePlayerPosition(initialX, initialZ);
	}

	void Terrain::UpdateChunks(float const& x, float const& z) noexcept
	{
		/*
		* Current chunk coordinates
		*/
		int currentChunkX{ (int)round(x / (float)m_chunkWidth) };
		int currentChunkZ{ (int)round(z / (float)m_chunkHeight) };

		/*
		* Delete chunks outside visualization distance
		*/
		for (auto it{ std::begin(m_chunks) }; it != std::end(m_chunks);)
		{
			int dx{ abs(it->first.first - currentChunkX) };
			int dz{ abs(it->first.second - currentChunkZ) };

			if (dx > m_numChunksToDisplay || dz > m_numChunksToDisplay)
			{
				it = m_chunks.erase(it);
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

				if (m_chunks.find(chunkKey) == m_chunks.end())
				{
					int xPos{ x * m_chunkWidth };
					int zPos{ z * m_chunkHeight };
					m_chunks[chunkKey] = std::make_unique<Chunk>(m_perlinParams, xPos, zPos, m_chunkWidth, m_chunkHeight);
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
				if (m_chunks.find(chunkKey) == m_chunks.end())
				{
					int xPos = chunkX * m_chunkWidth;
					int zPos = chunkZ * m_chunkHeight;
					m_chunks[chunkKey] = std::make_unique<Chunk>(m_perlinParams, xPos, zPos, m_chunkWidth, m_chunkHeight);
				}
				else{}
			}
		}

		UpdateChunks(x, z);
	}

	const float Terrain::GetHeightAtPos(float const& x, float const& z) noexcept
	{
		float height{ 0.0f };

		/*
		* Get chunk containing position (x,z)
		*/
		int currentChunkX = (int)round(x / (float)m_chunkWidth);
		int currentChunkZ = (int)round(z / (float)m_chunkHeight);

		std::pair <int, int> chunkKey = std::make_pair(currentChunkX, currentChunkZ);
		auto chunkIt = m_chunks.find(chunkKey);

		if (chunkIt != std::end(m_chunks))
		{
			height = chunkIt->second->GetHeightAtPos(x, z);
		}
		else{}

		return height;
	}

	void Terrain::RenderChunks(Shader const& terrainShader) const noexcept
	{
		terrainShader.UseProgram();

		for (const auto& [coord, chunk] : m_chunks)
		{	
			chunk->SetPerlinParams(m_perlinParams);
			glBindVertexArray(chunk->VAO());
			glDrawElements(GL_TRIANGLES, chunk->Indices().size(), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
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
		ImGui::DragFloat("Num octaves", &m_perlinParams.persistence, 0.05, 0.1, 10.0);

		ImGui::DragInt("Perlin Ridged num octaves", &m_perlinParams.multiNoiseNumOctaves, 1, 1, 50);
		ImGui::DragFloat("Frequency", &m_perlinParams.frequency, 0.05, 0.1, 50.0);
		ImGui::DragFloat("Lacunarity", &m_perlinParams.lacunarity, 0.05, 0.1, 50.0);

		ImGui::DragFloat("Scale", &m_perlinParams.scale, 0.05, 0.1, 50.0);
		ImGui::DragFloat("Amplitude", &m_perlinParams.lacunarity, 1.0, 0.0, 50.0);
		ImGui::End();
	}
}