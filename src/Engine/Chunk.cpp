#include "Chunk.hpp"

#include <glad/glad.h>
#include <noise/noise.h>

#include <iostream>

namespace Engine
{
    Chunk::Chunk(int const xPos, int const zPos, int const width, int const height) noexcept
    {
        m_position = glm::vec3(xPos, 0.0f, zPos);
        m_width = width;
        m_height = height;

        Generate();

        SetupMesh();
    }

    Chunk::~Chunk()
    {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }

    void Chunk::Generate() noexcept
    {
        noise::module::Perlin perlinNoise{};
        perlinNoise.SetOctaveCount(8);
        perlinNoise.SetPersistence(0.5);

        noise::module::RidgedMulti perlinRidgedMultiNoise{};
        perlinRidgedMultiNoise.SetOctaveCount(6);
        perlinRidgedMultiNoise.SetFrequency(0.1);
        perlinRidgedMultiNoise.SetLacunarity(2.0);

        m_vertices.reserve(m_width * m_height * 3);
        m_indices.reserve((m_width - 1) * (m_height - 1) * 6);

        float scale{ 0.05f };
        float amplitude{ 20.0f };

        for (unsigned int z{ 0 }; z < m_height; z++)
        {
            for (unsigned int x{ 0 }; x < m_width; x++)
            {
                float xPos{ m_position.x + static_cast<float>(x) };
                float zPos{ m_position.z + static_cast<float>(z) };

                float yPos{ static_cast<float>(perlinRidgedMultiNoise.GetValue(xPos * scale, 0.0, zPos * scale)) * amplitude };

                m_vertices.push_back(xPos);
                m_vertices.push_back(yPos);
                m_vertices.push_back(zPos);
            }
        }

        for (unsigned int i{ 0 }; i < m_height - 1; i++)
        {
            for (unsigned int j{ 0 }; j < m_width - 1; j++)
            {
                unsigned int topLeft{ i * m_width + j };
                unsigned int topRight{ topLeft + 1 };

                unsigned int bottomLeft{ topLeft + m_width };
                unsigned int bottomRight{ bottomLeft + 1 };

                m_indices.push_back(topLeft);
                m_indices.push_back(bottomLeft);
                m_indices.push_back(topRight);

                m_indices.push_back(topRight);
                m_indices.push_back(bottomLeft);
                m_indices.push_back(bottomRight);
            }
        }
    }

    void Chunk::SetupMesh() noexcept
    {
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

        // Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    const float Chunk::GetHeightAtPos(float const& x, float const& z) const noexcept
    {
        float height{ 0.0f };
        float playerHeight{ 2.7f };

        // Get vertices index around (x,y) position
        int x0{ static_cast<int>(floor(abs(x))) };
        int x1{ x0 + 1 };
        int z0{ static_cast<int>(floor(abs(z))) };
        int z1{ z0 + 1 };

        // Interpolate terrain height at (x, y) position
        float fx{ abs(x) - x0 };
        float fz{ abs(z) - z0 };
        float h00{ m_vertices[(z0 * m_width + x0) * 3 + 1] };
        float h01{ m_vertices[(z1 * m_width + x0) * 3 + 1] };
        float h10{ m_vertices[(z0 * m_width + x1) * 3 + 1] };
        float h11{ m_vertices[(z1 * m_width + x1) * 3 + 1] };

        height = h00 * (1 - fx) * (1 - fz) + h10 * fx * (1 - fz) + h01 * (1 - fx) * fz + h11 * fx * fz + playerHeight;

        return height;
    }
}