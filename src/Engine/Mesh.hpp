#pragma once

#include "Engine/Shader.hpp"

#include <string_view>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoords;

		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	class Mesh
	{
	public:
		Mesh() = delete;
		~Mesh() = default;

		/*
		* ONLY CONSTRUCTOR ALLOWED
		*/
		Mesh(std::vector <Vertex> const& vertices, std::vector <unsigned int> const& indices) noexcept;

		[[nodiscard]] inline const std::vector <Vertex> Vertices() const noexcept { return m_vertices; }
		[[nodiscard]] inline const std::vector <unsigned int> Indices() const noexcept { return m_indices; }
		[[nodiscard]] inline const unsigned int VAO() const noexcept { return m_VAO; }

	private:
		std::vector <Vertex> m_vertices;
		std::vector <unsigned int> m_indices;

		/*
		* BUFFERS
		*/
		unsigned int m_VAO;
		unsigned int m_VBO;
		unsigned int m_EBO;

		void SetMesh() noexcept;
	};
}
