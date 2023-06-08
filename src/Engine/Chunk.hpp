#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine
{
    class Chunk
    {
    public:
        Chunk() = default;
        Chunk(float const &xPos, float const &zPos, unsigned int const width, unsigned int const height) noexcept;
        ~Chunk();

        void Generate() noexcept;
        void SetupMesh() noexcept;

        const float GetHeightAtPos(float const& x, float const& z) const noexcept;

        [[nodiscard]] inline const std::vector <float> Vertices() const noexcept { return m_vertices; }
        [[nodiscard]] inline const std::vector <unsigned int> Indices() const noexcept { return m_indices; }
        [[nodiscard]] inline const glm::vec3 Position() const noexcept { return m_position; }
        [[nodiscard]] inline const unsigned int VAO() const noexcept { return m_VAO; }

    private:
        glm::vec3 m_position;
        unsigned int m_width;
        unsigned int m_height;
        std::vector<float> m_vertices;
        std::vector<unsigned int> m_indices;
        unsigned int m_VAO, m_VBO, m_EBO;
    };
}