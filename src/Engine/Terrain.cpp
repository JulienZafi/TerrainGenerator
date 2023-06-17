#include "Terrain.hpp"
#include "Window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Engine
{
	Terrain::Terrain() noexcept
	{
		float initialX = (float)CHUNK_WIDTH * (float)NUM_CHUNCKS_TO_DISPLAY / 2.0f;
		float initialZ = (float)CHUNK_HEIGHT * (float)NUM_CHUNCKS_TO_DISPLAY / 2.0f;

		UpdatePlayerPosition(initialX, initialZ);
	}

	void Terrain::UpdateChunks(float const& x, float const& z) noexcept
	{
		/*
		* Current chunk coordinates
		*/
		int currentChunkX{ (int)round(x / (float)CHUNK_WIDTH) };
		int currentChunkZ{ (int)round(z / (float)CHUNK_HEIGHT) };

		/*
		* Delete chunks outside visualization distance
		*/
		for (auto it{ std::begin(m_chunks) }; it != std::end(m_chunks);)
		{
			int dx{ abs(it->first.first - currentChunkX) };
			int dz{ abs(it->first.second - currentChunkZ) };

			if (dx > NUM_CHUNCKS_TO_DISPLAY || dz > NUM_CHUNCKS_TO_DISPLAY)
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
		for (int x{ currentChunkX - NUM_CHUNCKS_TO_DISPLAY }; x < currentChunkX + NUM_CHUNCKS_TO_DISPLAY; ++x)
		{
			for (int z{ currentChunkZ - NUM_CHUNCKS_TO_DISPLAY }; z < currentChunkZ + NUM_CHUNCKS_TO_DISPLAY; ++z)
			{
				auto chunkKey{ std::make_pair(x, z) };

				if (m_chunks.find(chunkKey) == m_chunks.end())
				{
					int xPos{ x * CHUNK_WIDTH };
					int zPos{ z * CHUNK_HEIGHT };
					m_chunks[chunkKey] = std::make_unique<Chunk>(xPos, zPos, CHUNK_WIDTH, CHUNK_HEIGHT);
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
		int currentChunkX = (int)round(x / (float)CHUNK_WIDTH);
		int currentChunkZ = (int)round(z / (float)CHUNK_HEIGHT);

		/*
		* Generate missing chunks around player
		*/
		for (int dx = -NUM_CHUNCKS_TO_DISPLAY; dx <= NUM_CHUNCKS_TO_DISPLAY; ++dx)
		{
			for (int dz = -NUM_CHUNCKS_TO_DISPLAY; dz <= NUM_CHUNCKS_TO_DISPLAY; ++dz)
			{
				int chunkX = currentChunkX + dx;
				int chunkZ = currentChunkZ + dz;
				std::pair<int, int> chunkKey = std::make_pair(chunkX, chunkZ);

				if (m_chunks.find(chunkKey) == m_chunks.end())
				{
					int xPos = chunkX * CHUNK_WIDTH;
					int zPos = chunkZ * CHUNK_HEIGHT;
					m_chunks[chunkKey] = std::make_unique<Chunk>(xPos, zPos, CHUNK_WIDTH, CHUNK_HEIGHT);
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
		int currentChunkX = (int)round(x / (float)CHUNK_WIDTH);
		int currentChunkZ = (int)round(z / (float)CHUNK_HEIGHT);

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
			glBindVertexArray(chunk->VAO());
			glDrawElements(GL_TRIANGLES, chunk->Indices().size(), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}
}