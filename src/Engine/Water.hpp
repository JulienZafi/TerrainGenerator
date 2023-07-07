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

		void UpdateMesh(glm::vec3 const &camPosition) noexcept;

		void BindFrameBuffer(unsigned int const frameBuffer, unsigned int const width, unsigned int const height) const noexcept;

		void BindReflectionFrameBuffer() const noexcept;
		void BindRefractionFrameBuffer() const noexcept;

		unsigned int LoadTextureFromFile(std::string_view const& path) const noexcept;

		void SetLightProperties(glm::vec3 const& lightDirection, glm::vec3 const& lightColor) noexcept;
		void SetTime(float const& deltaTime) noexcept;

		void UnbindCurrentFrameBuffer() const noexcept;

		void Render(Shader const& shader, glm::mat4 const& projection, glm::mat4 const& view, glm::mat4 const& model) noexcept;

		void ShowGUI() noexcept;

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

		glm::vec3 m_cameraPosition;
		glm::vec3 m_lightColor;
		glm::vec3 m_lightDirection;
		glm::vec3 m_waterColor;
		float m_waveLevel;
		float m_waveSpeed;
		float m_moveFactor;
		float m_tiling;
		float m_refractiveFactor;
		float m_shineDamper;
		float m_reflectivity;
		float m_deltaTime;

		float m_zNear;
		float m_zFar;
		float m_waterDepthFactor;
	};
};
