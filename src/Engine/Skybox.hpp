#pragma once

#include <string>
#include <vector>

const std::vector <std::string> cubeMap
{ 
	"skybox//right.jpg",
	"skybox//left.jpg",
	"skybox//top.jpg",
	"skybox//bottom.jpg",
    "skybox//front.jpg",
	"skybox//back.jpg"
};

constexpr float const skyboxVertices[]
{
    // positions          
    -1000.0f,  1000.0f, -1000.0f,
    -1000.0f, -1000.0f, -1000.0f,
     1000.0f, -1000.0f, -1000.0f,
     1000.0f, -1000.0f, -1000.0f,
     1000.0f,  1000.0f, -1000.0f,
    -1000.0f,  1000.0f, -1000.0f,

    -1000.0f, -1000.0f,  1000.0f,
    -1000.0f, -1000.0f, -1000.0f,
    -1000.0f,  1000.0f, -1000.0f,
    -1000.0f,  1000.0f, -1000.0f,
    -1000.0f,  1000.0f,  1000.0f,
    -1000.0f, -1000.0f,  1000.0f,

     1000.0f, -1000.0f, -1000.0f,
     1000.0f, -1000.0f,  1000.0f,
     1000.0f,  1000.0f,  1000.0f,
     1000.0f,  1000.0f,  1000.0f,
     1000.0f,  1000.0f, -1000.0f,
     1000.0f, -1000.0f, -1000.0f,

    -1000.0f, -1000.0f,  1000.0f,
    -1000.0f,  1000.0f,  1000.0f,
     1000.0f,  1000.0f,  1000.0f,
     1000.0f,  1000.0f,  1000.0f,
     1000.0f, -1000.0f,  1000.0f,
    -1000.0f, -1000.0f,  1000.0f,

    -1000.0f,  1000.0f, -1000.0f,
     1000.0f,  1000.0f, -1000.0f,
     1000.0f,  1000.0f,  1000.0f,
     1000.0f,  1000.0f,  1000.0f,
    -1000.0f,  1000.0f,  1000.0f,
    -1000.0f,  1000.0f, -1000.0f,

    -1000.0f, -1000.0f, -1000.0f,
    -1000.0f, -1000.0f,  1000.0f,
     1000.0f, -1000.0f, -1000.0f,
     1000.0f, -1000.0f, -1000.0f,
    -1000.0f, -1000.0f,  1000.0f,
     1000.0f, -1000.0f,  1000.0f
};

namespace Engine
{
    class Shader;

	class Skybox
	{
	public:
		Skybox();
		~Skybox() = default;

        void Bind() noexcept;
		void LoadTextures() noexcept;

        void Render(Shader const& shader, glm::mat4 const& projection, glm::mat4 const& view) const noexcept;

	private:
		unsigned int m_textureID;
		unsigned int m_VAO;
        unsigned int m_VBO;

        unsigned int m_scale;
	};
};