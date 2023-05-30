#include "Terrain.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Engine
{
	Terrain::Terrain() noexcept
	{		
		m_playerX = 0;
		m_playerZ = 0;

		GenerateChunks();
	}

	void Terrain::GenerateChunks() noexcept
	{
		int width{ CHUNK_WIDTH * NUM_CHUNCKS_TO_DISPLAY };
		int height{ CHUNK_HEIGHT * NUM_CHUNCKS_TO_DISPLAY };

		for (int x{ 0 }; x <= width; x += CHUNK_WIDTH)
		{
			for (int z{ 0 }; z <= height; z += (float)CHUNK_HEIGHT)
			{
				m_chunks[std::make_pair(x, z)] = std::make_unique<Chunk>(x, z, CHUNK_WIDTH, CHUNK_HEIGHT);
			}
		}
	}

	void Terrain::UpdateChunks() noexcept
	{
		/*
		* Current chunk coordinates
		*/
		float currentChunkX{ m_playerX / (float)CHUNK_WIDTH };
		float currentChunkZ{ m_playerZ / (float)CHUNK_HEIGHT };

		/*
		* Delete chunks outside visualization distance
		*/
		for (auto it{ std::begin(m_chunks) }; it != std::end(m_chunks);)
		{
			float dx{ abs(it->first.first - currentChunkX) };
			float dz{ abs(it->first.second - currentChunkZ) };

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
		for (int x{ (int)currentChunkX - NUM_CHUNCKS_TO_DISPLAY }; x <= (int)currentChunkX + NUM_CHUNCKS_TO_DISPLAY; ++x)
		{
			for (int z{ (int)currentChunkZ - NUM_CHUNCKS_TO_DISPLAY }; z <= (int)currentChunkZ + NUM_CHUNCKS_TO_DISPLAY; ++z)
			{
				auto chunkKey{ std::make_pair((float)x, (float)z) };

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
		m_playerX = x;
		m_playerZ = GetHeightAtPos(x, z);

		UpdateChunks();
	}

	const float Terrain::GetHeightAtPos(float const& x, float const& z) const noexcept
	{
		float height{ 0.0f };

		/*
		* Get chunk containing position (x,z)
		*/
		int chunkX = (int)x / CHUNK_WIDTH;
		int chunkZ = (int)z / CHUNK_HEIGHT;

		std::pair <int, int> chunkKey = std::make_pair(chunkX, chunkZ);
		auto chunkIt = m_chunks.find(chunkKey);

		/*
		* Get height if chunk exists
		*/
		if (chunkIt != m_chunks.end()) 
		{
			height = chunkIt->second->GetHeightAtPos(x, z);
		}
		else
		{
			height = z;
		}

		return height;
	}

	void Terrain::RenderChunks(Shader const& terrainShader) const noexcept
	{
		terrainShader.UseProgram();

		for (const auto& chunksPair : m_chunks)
		{
			const auto& chunk{ chunksPair.second };
			
			glBindVertexArray(chunk->VAO());
			glDrawElements(GL_TRIANGLES, chunk->Indices().size(), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}
}