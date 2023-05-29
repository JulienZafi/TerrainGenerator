
#include "Shader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace Engine
{
	Shader::Shader(std::string_view const& vertexShaderPath, std::string_view const& fragmentShaderPath) noexcept
	{
		unsigned int vertexShader{ std::move(CreateShader(TYPE::VERTEX, vertexShaderPath)) };
		unsigned int fragmentShader{ std::move(CreateShader(TYPE::FRAGMENT, fragmentShaderPath)) };

		CreateProgram(vertexShader, fragmentShader);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	unsigned int Shader::CreateShader(TYPE const type, std::string_view const& path) noexcept
	{
		unsigned int shader{};
		switch (type)
		{
		case TYPE::VERTEX:
			shader = glCreateShader(GL_VERTEX_SHADER);
			break;
		case TYPE::FRAGMENT:
			shader = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		default:
			break;
		}

		std::string source{ std::move(GetShaderSource(path)) };
		const char* shaderSource{ source.c_str() };
		glShaderSource(shader, 1, &shaderSource, nullptr);
		glCompileShader(shader);

		CheckCompileStatus(type, shader);

		return shader;
	}

	void Shader::CreateProgram(unsigned int const vertexShader, unsigned int const fragmentShader) noexcept
	{
		m_programID = glCreateProgram();
		glAttachShader(m_programID, vertexShader);
		glAttachShader(m_programID, fragmentShader);
		glLinkProgram(m_programID);

		CheckCompileStatus(TYPE::PROGRAM);
	}

	std::string Shader::GetShaderSource(std::string_view const& path) const noexcept
	{
		std::ifstream file{};
		file.exceptions(std::ios::failbit | std::ios::badbit);
		std::string res{};
		try
		{
			file.open(path.data(), std::ios::in);
			std::stringstream stream{};
			stream << file.rdbuf();
			file.close();

			res = std::move(stream.str());
		}
		catch (std::ios::failure const& ex)
		{
			std::cout << "ERROR::SHADER::SOURCE::FAILED_TO_READ_FILE : \n" << path.data() << std::endl;
		}

		return res;
	}

	void Shader::CheckCompileStatus(TYPE const type, unsigned int const shader) noexcept
	{
		char infoLog[512];

		if (type == TYPE::PROGRAM)
		{
			glGetProgramiv(m_programID, GL_LINK_STATUS, &_succeed);
			if (!_succeed)
			{
				glGetProgramInfoLog(m_programID, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::FAILED_TO_LINK\n" << infoLog << std::endl;
			}
			else{}
		}
		else
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &_succeed);
			if (!_succeed)
			{
				glGetShaderInfoLog(shader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::FAILED_TO_COMPILE\n" << infoLog << std::endl;
			}
			else {}
		}		
	}
}