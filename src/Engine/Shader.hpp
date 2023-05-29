#pragma once

#include <string>
#include <string_view>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	enum class TYPE{ VERTEX = 0, FRAGMENT, PROGRAM };

	class Shader
	{
	public:
		Shader() = default;
		~Shader() = default;
		
		/*
		* INITIALIZE GPU PROGRAM
		*/
		Shader(std::string_view const& vertexShaderPath, std::string_view const& fragmentShaderPath) noexcept;
		unsigned int CreateShader(TYPE const type, std::string_view const& path) noexcept;
		void CreateProgram(unsigned int const vertexShader, unsigned int const fragmentShader) noexcept;
		std::string GetShaderSource(std::string_view const& path) const noexcept;

		/*
		* USE GPU PROGRAM
		*/
		template <typename T>
		void SetUniform(std::string_view const& name, T const& value) const noexcept
		{
			int location{ glGetUniformLocation(m_programID, name.data()) };
			glUniform1i(location, value);
		}

		template<>
		void SetUniform<float>(std::string_view const& name, float const& value) const noexcept
		{
			int location{ glGetUniformLocation(m_programID, name.data()) };
			glUniform1f(location, value);
		}

		template<>
		void SetUniform<glm::mat4>(std::string_view const& name, glm::mat4 const& value) const noexcept
		{
			int location{ glGetUniformLocation(m_programID, name.data()) };
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
		}

		template<>
		void SetUniform<glm::vec3>(std::string_view const& name, glm::vec3 const& value) const noexcept
		{
			int location{ glGetUniformLocation(m_programID, name.data()) };
			glUniform3f(location, value.x, value.y, value.z);
		}
		
		[[nodiscard]] inline void UseProgram() const noexcept { glUseProgram(m_programID); }

	private:
		unsigned int m_programID;

		int _succeed;

		void CheckCompileStatus(TYPE const type, unsigned int const shader = 0) noexcept;
	};

}