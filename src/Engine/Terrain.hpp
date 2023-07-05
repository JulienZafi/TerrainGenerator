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

		void Render(Shader const& shader, glm::mat4 const& projection, glm::mat4 const& view, glm::mat4 const& model) const noexcept;

		void ShowGUI() noexcept;

		[[nodiscard]] inline const unsigned int Width() const noexcept{ return (m_numChunksToDisplay * m_chunkWidth); }
		[[nodiscard]] inline const unsigned int Height() const noexcept{ return (m_numChunksToDisplay * m_chunkHeight); }

		void SetLight(glm::vec3 const& lightColor, glm::vec3 const& lightDirection) noexcept;
		void SetClipPlane(glm::vec4 const& clipPlane) noexcept;

	private:
		std::map <std::pair <int, int>, std::unique_ptr <Mesh>> m_meshes;

		std::unordered_map<TextureType, unsigned int> m_textures;
		unsigned int m_texture;

		int m_numChunksToDisplay;
		int m_chunkWidth;
		int m_chunkHeight;

		PerlinParams m_perlinParams;

		glm::vec3 m_lightColor;
		glm::vec3 m_lightDirection;
		glm::vec4 m_clipPlane;
	};
}