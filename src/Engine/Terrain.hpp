#pragma once

#include "Camera.hpp"
#include "Chunk.hpp"
#include "Shader.hpp"

#include <map>
#include <memory>
#include <vector>

#define NUM_CHUNCKS_TO_DISPLAY	2
#define CHUNK_WIDTH				50
#define CHUNK_HEIGHT			50

namespace Engine
{	
	class Terrain
	{
	public:
		Terrain() noexcept;
		~Terrain() = default;

		void UpdateChunks(float const& x, float const& z) noexcept;
		void UpdatePlayerPosition(float const& x, float const& z) noexcept;

		const float GetHeightAtPos(float const& x, float const& z) noexcept;
		void RenderChunks(Shader const& terrainShader) const noexcept;

		[[nodiscard]] inline const unsigned int Width(){ return (NUM_CHUNCKS_TO_DISPLAY * CHUNK_WIDTH); }
		[[nodiscard]] inline const unsigned int Height(){ return (NUM_CHUNCKS_TO_DISPLAY * CHUNK_HEIGHT); }

	private:
		std::map <std::pair <int, int>, std::unique_ptr <Chunk>> m_chunks;
	};
}