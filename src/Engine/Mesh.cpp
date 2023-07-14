#include "Mesh.hpp"

#include <iostream>

namespace Engine
{
	Mesh::Mesh(std::vector <Vertex> const& vertices, std::vector <unsigned int> const& indices) noexcept
	{
		m_vertices = vertices;
		m_indices = indices;

		SetMesh();
	}

	void Mesh::SetMesh() noexcept
	{
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, std::size(m_vertices) * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::size(m_indices) * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

		/*
		* DEFINE BUFFER DATA
		*/
		// POSITIONS
		////////////
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);

		// NORMALS
		//////////
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		// TEXTURE COORDS
		/////////////////
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));
		glEnableVertexAttribArray(2);

		// TANGENT
		//////////
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		glEnableVertexAttribArray(3);

		// BITANGENT
		////////////
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
		glEnableVertexAttribArray(4);

		glBindVertexArray(0);
	}
}