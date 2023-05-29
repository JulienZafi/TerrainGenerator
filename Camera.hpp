#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Engine
{
/*
* EULER ANGLES
*/
#define YAW				  0.0f
#define PITCH			  0.0f

/*
* MOUSE CONSTANTS
*/
#define ZOOM			  100.0f
#define SENSITIVITY		  0.1f
#define SPEED			  20.0f

	enum class MOTION{ FORWARD = 0, BACKWARD, LEFT, RIGHT };

	class Camera
	{
	public:
		Camera() = delete;
		~Camera() = default;

		Camera(glm::vec3 const& position, glm::vec3 const& upDirection = glm::vec3(0.0f, 1.0f, 0.0f), float const& yaw = YAW, float const& pitch = PITCH) noexcept;

		/*
		* PROCESS USER INPUTS
		*/
		void ProcessKeyboard(MOTION const motion, float const &deltaTime) noexcept;
		void ProcessCursorPos(float const& xoffset, float const& yoffset, bool const constrainPitch = true) noexcept;
		void ProcessScroll(float const& yoffset) noexcept;

		/*
		* GETTERS
		*/
		[[nodiscard]] inline const glm::vec3 Position() const noexcept { return m_position; }
		[[nodiscard]] inline const glm::vec3 FrontDirection() const noexcept { return m_frontDirection; }
		[[nodiscard]] inline const float Zoom() const noexcept { return m_zoom; }
		[[nodiscard]] inline const glm::mat4 ViewMatrix() const noexcept { return glm::lookAt(m_position, m_position + m_frontDirection, m_upDirection); }

		/*
		* SETTERS
		*/
		void SetPosition(glm::vec3 const &position) noexcept;

	private:
		/*
		* POSITIONS & DIRECTIONS
		*/
		glm::vec3 m_position;
		
		glm::vec3 m_frontDirection;
		glm::vec3 m_upDirection;
		glm::vec3 m_rightDirection;
		glm::vec3 m_worldUp;

		/*
		* EULER ANGLES
		*/
		float m_yaw;
		float m_pitch;

		/*
		* OPTIONS
		*/
		float m_speed;
		float m_sensitivity;
		float m_zoom;

		void Update() noexcept;
	};
}