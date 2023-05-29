#include "Terrain.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Engine
{
	Terrain::Terrain(unsigned int width, unsigned int height) noexcept
	{
		m_width = width;
		m_height = height;
		
		m_playerX = 0;
		m_playerZ = 0;
	}

	void Terrain::UpdateChunks() noexcept
	{
		/*
		* Current chunk coordinates
		*/
		unsigned int currentChunkX{ (unsigned int)m_playerX / m_width };
		unsigned int currentChunkZ{ (unsigned int)m_playerZ / m_height };

		/*
		* Delete chunks outside visualization distance
		*/
		for (auto it{ std::begin(m_chunks) }; it != std::end(m_chunks);)
		{
			unsigned int dx{ static_cast <unsigned int> (abs(it->first.first - currentChunkX)) };
			unsigned int dz{ static_cast <unsigned int> (abs(it->first.second - currentChunkZ)) };

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
		for (unsigned int x{ currentChunkX - NUM_CHUNCKS_TO_DISPLAY }; x <= currentChunkX + NUM_CHUNCKS_TO_DISPLAY; ++x)
		{
			for (unsigned int z{ currentChunkZ - NUM_CHUNCKS_TO_DISPLAY }; z <= currentChunkZ + NUM_CHUNCKS_TO_DISPLAY; ++z)
			{
				auto chunkKey{ std::make_pair((float)x, (float)z) };

				if (m_chunks.find(chunkKey) == m_chunks.end())
				{
					unsigned int xPos{ x * m_width };
					unsigned int zPos{ z * m_height };
					m_chunks[chunkKey] = std::make_unique<Chunk>(xPos, zPos, m_width, m_height);
				}
				else{}
			}
		}
	}

	void Terrain::UpdatePlayerPosition(float const& x, float const& z) noexcept
	{
		m_playerX = x;
		m_playerZ = z;

		UpdateChunks();
	}

	const float Terrain::GetHeightAtPos(float const& x, float const& z) const noexcept
	{
		float height{ 0.0f };

		/*
		* Get chunk containing position (x,z)
		*/
		unsigned int chunkX = (unsigned int)x / m_width;
		unsigned int chunkZ = (unsigned int)z / m_height;

		std::pair <float, float> chunkKey = std::make_pair((float)chunkX, (float)chunkZ);
		auto chunkIt = m_chunks.find(chunkKey);

		if (chunkIt != m_chunks.end()) 
		{
			/*
			* Get height if chunk exists
			*/
			height = chunkIt->second->GetHeightAtPos(x, z);
		}
		else{}

		return height;
	}

	void Terrain::RenderChunks(Shader const& terrainShader) const noexcept
	{
		terrainShader.UseProgram();

		for (const auto& chunksPair : m_chunks)
		{
			const auto& chunk{ chunksPair.second };
			
			glBindVertexArray(chunk->VAO());
			std::cout << chunk->VAO() << std::endl;

			// World transformation
			//glm::mat4 model = glm::translate(glm::mat4(1.0f), chunk->Position());
			//terrainShader.SetUniform<glm::mat4>("u_model", model);

			glDrawElements(GL_TRIANGLES, chunk->Indices().size(), GL_UNSIGNED_INT, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
	}
}