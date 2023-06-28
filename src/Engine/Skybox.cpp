#include "Skybox.hpp"

#include <glad/glad.h>

#include <iostream>

#include <stb_image.h>

namespace Engine
{
	Skybox::Skybox()
	{
        Bind();

        stbi_set_flip_vertically_on_load(false);
        LoadTextures();
	}
    
    void Skybox::LoadTextures() noexcept
	{
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

        int width{}, height{}, numComponents{};
        for (unsigned int i = 0; i < std::size(cubeMap); i++)
        {
            unsigned char* data = stbi_load(cubeMap[i].c_str(), &width, &height, &numComponents, 0);
            if (data)
            {
                GLenum format{};
                if (numComponents == 1)
                {
                    format = GL_RED;
                }
                else if (numComponents == 3)
                {
                    format = GL_RGB;
                }
                else if (numComponents == 4)
                {
                    format = GL_RGBA;
                }
                else {}

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << cubeMap[i] << std::endl;
            }
            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

    void Skybox::Bind() noexcept
    {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void Skybox::Render() const noexcept
    {
        // skybox cube
        glBindVertexArray(m_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }
}