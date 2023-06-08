#include "Terrain.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
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
		for (int x{ (int)currentChunkX - NUM_CHUNCKS_TO_DISPLAY }; x < (int)currentChunkX + NUM_CHUNCKS_TO_DISPLAY; ++x)
		{
			for (int z{ (int)currentChunkZ - NUM_CHUNCKS_TO_DISPLAY }; z < (int)currentChunkZ + NUM_CHUNCKS_TO_DISPLAY; ++z)
			{
				auto chunkKey{ std::make_pair(x, z) };

				if (m_chunks.find(chunkKey) == m_chunks.end())
				{
					float xPos{ (float)x * (float)CHUNK_WIDTH };
					float zPos{ (float)z * (float)CHUNK_HEIGHT };
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
		float chunkX = x / (float)CHUNK_WIDTH;
		float chunkZ = z / (float)CHUNK_HEIGHT;

		std::pair <int, int> chunkKey = std::make_pair((int)chunkX, (int)chunkZ);
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
			//height = z;
		}

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