#pragma once

#include <memory>
#include <string>
#include <string_view>

constexpr std::string_view const DUDV_FILE{ "textures//water_dudv.png" };
constexpr std::string_view const NORMAL_FILE{ "textures//water_normalmap.png" };

#define REFLECTION_WIDTH 1920
#define REFLECTION_HEIGHT 1080

#define REFRACTION_WIDTH 1920
#define REFRACTION_HEIGHT 1080

#define OFFSET_POS 25

namespace Engine
{
	class Water
	{
	public:
		Water() = delete;
		~Water() = default;

		Water(float const xPos, float const& zpos, unsigned int const width, unsigned int const height) noexcept;

		void InitFrameBuffers() noexcept;
		unsigned int CreateFrameBuffer() noexcept;
		unsigned int CreateTexture(unsigned int const width, unsigned int const height) noexcept;
		unsigned int CreateDepthTexture(unsigned int const width, unsigned int const height) noexcept;
		unsigned int CreateDepthBuffer(unsigned int const width, unsigned int const height) noexcept;
		unsigned int CreateRenderBufferAttachment(int width, int height) noexcept;

		void UpdateMesh(glm::vec3 const &camPosition) noexcept;

		void BindFrameBuffer(unsigned int const frameBuffer, unsigned int const width, unsigned int const height) const noexcept;

		void BindReflectionFrameBuffer() const noexcept;
		void BindRefractionFrameBuffer() const noexcept;

		unsigned int LoadTextureFromFile(std::string_view const& path) const noexcept;

		[[nodiscard]] inline const unsigned int ReflectionTexture() const noexcept { return m_reflectionTexture; }
		[[nodiscard]] inline const unsigned int RefractionTexture() const noexcept { return m_refractionTexture; }
		[[nodiscard]] inline const unsigned int RefractionDepthTexture() const noexcept { return m_refractionDepthTexture; }
		[[nodiscard]] inline const unsigned int DudvMap() const noexcept { return m_dudvMap; }
		[[nodiscard]] inline const unsigned int NormalMap() const noexcept { return m_normalMap; }

		void UnbindCurrentFrameBuffer() const noexcept;

		void Render(Shader const& terrainShader) const noexcept;

	private:
		std::unique_ptr <Mesh> m_mesh;
	
		float m_xpos;
		float m_zpos;

		unsigned int m_width;
		unsigned int m_height;

		unsigned int m_reflectionFrameBuffer;
		unsigned int m_reflectionTexture;
		unsigned int m_reflectionDepthBuffer;

		unsigned int m_refractionFrameBuffer;
		unsigned int m_refractionTexture;
		unsigned int m_refractionDepthTexture;

		unsigned int m_dudvMap;
		unsigned int m_normalMap;
	};
};