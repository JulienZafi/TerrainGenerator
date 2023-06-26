#pragma once

#include <memory>

#define REFLECTION_WIDTH 320
#define REFLECTION_HEIGHT 180

#define REFRACTION_WIDTH 1280
#define REFRACTION_HEIGHT 720

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

		void BindFrameBuffer(unsigned int const frameBuffer, unsigned int const width, unsigned int const height) const noexcept;

		void BindReflectionFrameBuffer() const noexcept;
		void BindRefractionFrameBuffer() const noexcept;

		[[nodiscard]] inline const unsigned int ReflectionTexture() const noexcept { return m_reflectionTexture; }
		[[nodiscard]] inline const unsigned int RefractionTexture() const noexcept { return m_refractionTexture; }
		[[nodiscard]] inline const unsigned int RefractionDepthTexture() const noexcept { return m_refractionDepthTexture; }

		void UnbindCurrentFrameBuffer() const noexcept;

		void Render(Shader const& terrainShader) const noexcept;

	private:
		std::unique_ptr <Mesh> m_mesh;

		unsigned int m_width;
		unsigned int m_height;

		unsigned int m_reflectionFrameBuffer;
		unsigned int m_reflectionTexture;
		unsigned int m_reflectionDepthBuffer;

		unsigned int m_refractionFrameBuffer;
		unsigned int m_refractionTexture;
		unsigned int m_refractionDepthTexture;
	};
};