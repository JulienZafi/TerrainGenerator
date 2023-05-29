#pragma once

#include "Chunk.hpp"
#include "Shader.hpp"

#include <map>
#include <memory>
#include <vector>

#define NUM_CHUNCKS_TO_DISPLAY	3

namespace Engine
{	
	class Terrain
	{
	public:
		Terrain() = default;
		~Terrain() = default;
		Terrain(unsigned int width, unsigned int height) noexcept;

		void UpdateChunks() noexcept;
		void UpdatePlayerPosition(float const& x, float const& z) noexcept;

		const float GetHeightAtPos(float const& x, float const& z) const noexcept;
		void RenderChunks(Shader const& terrainShader) const noexcept;

	private:
		unsigned int m_width;
		unsigned int m_height;
		float m_playerX;
		float m_playerZ;
		std::map <std::pair <float, float>, std::unique_ptr <Chunk>> m_chunks;
	};
}