#pragma once

#include "Camera.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

constexpr std::string const TEXTURES_PATH{ "textures//" };

#define NUM_CHUNCKS_TO_DISPLAY	6
#define CHUNK_WIDTH				50
#define CHUNK_HEIGHT			50

namespace Engine
{	
	struct PerlinParams
	{
		int numOctaves;
		float persistence;
		int multiNoiseNumOctaves;
		float frequency;
		float lacunarity;
		float scale;
		float amplitude;
	};

	enum class TextureType
	{
		GRASS = 0,
		ROCK,
		SAND,
		SNOW,
		WATER
	};

	class Terrain
	{
	public:
		Terrain() noexcept;
		~Terrain() = default;

		std::unique_ptr <Mesh> ComputeVertices(float const& xPos, float const& zPos) noexcept;

		unsigned int LoadTextureFromFile(std::string const& path) const noexcept;
		void LoadTexture(std::string const& fileName, TextureType const type) noexcept;
		void BindTextures(Shader const& shader) const noexcept;

		void Update(float const& x, float const& z) noexcept;
		void UpdatePlayerPosition(float const& x, float const& z) noexcept;

		void Render(Shader const& terrainShader) const noexcept;

		void ShowGUI() noexcept;

		[[nodiscard]] inline const unsigned int Width(){ return (m_numChunksToDisplay * m_chunkWidth); }
		[[nodiscard]] inline const unsigned int Height(){ return (m_numChunksToDisplay * m_chunkHeight); }

	private:
		std::map <std::pair <int, int>, std::unique_ptr <Mesh>> m_meshes;

		std::unordered_map<TextureType, unsigned int> m_textures;
		unsigned int m_texture;

		int m_numChunksToDisplay;
		int m_chunkWidth;
		int m_chunkHeight;

		PerlinParams m_perlinParams;
	};
}