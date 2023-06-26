#pragma once

#include <memory>

#define WATER_DIM	256

namespace Engine
{
	class Water
	{
	public:
		Water() = delete;
		~Water() = default;

		Water(float const xPos, float const& zpos, unsigned int const width, unsigned int const height) noexcept;

		void Render(Shader const& terrainShader) const noexcept;

	private:
		std::unique_ptr <Mesh> m_mesh;
	};
};