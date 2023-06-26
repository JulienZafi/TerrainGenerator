#include "Mesh.hpp"
#include "Water.hpp"

namespace Engine
{
	Water::Water(float const xPos, float const& zpos, unsigned int const width, unsigned int const height) noexcept
	{
		std::vector <Vertex> vertices{};
		for (unsigned int z{ 0 }; z < width - 1; ++z)
		{
			for (unsigned int x{ 0 }; x < height - 1; ++x)
			{
				Vertex vertex{};
				vertex.position.x = x;
				vertex.position.y = 0;

				//vertex.position.z = z - (float)height / 2.0f;
				vertex.position.z = z;

				vertex.textureCoords.x = static_cast<float>(x) / (float)width;
				vertex.textureCoords.y = static_cast<float>(z) / (float)height;
				vertices.push_back(vertex);
			}
		}

		std::vector <unsigned int> indices{};
		for (unsigned int i{ 0 }; i < width - 1; ++i)
		{
			for (unsigned int j{ 0 }; j < height - 1; ++j)
			{
				unsigned int topLeft{ i * width + j };
				unsigned int topRight{ topLeft + 1 };

				unsigned int bottomLeft{ (i + 1) * width + j };
				unsigned int bottomRight{ bottomLeft + 1 };

				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);

				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}
		}

		m_mesh = std::make_unique <Mesh>(vertices, indices);
	}

	void Water::Render(Shader const& terrainShader) const noexcept
	{
		glBindVertexArray(m_mesh->VAO());
		glDrawElements(GL_TRIANGLES, std::size(m_mesh->Indices()), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
};