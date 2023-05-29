#include "Camera.hpp"

namespace Engine
{
	Camera::Camera(glm::vec3 const& position, glm::vec3 const& upDirection, float const& yaw, float const& pitch) noexcept
	{
		m_position = position;
		m_frontDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		m_worldUp = upDirection;
		m_yaw = yaw;
		m_pitch = pitch;

		m_zoom = ZOOM;
		m_sensitivity = SENSITIVITY;
		m_speed = SPEED;

		Update();
	}

	void Camera::ProcessKeyboard(MOTION const motion, float const& deltaTime) noexcept
	{
		float velocity{ deltaTime * m_speed };
		switch (motion)
		{
		case MOTION::FORWARD:
			m_position += m_frontDirection * velocity;
			break;
		case MOTION::BACKWARD:
			m_position -= m_frontDirection * velocity;
			break;
		case MOTION::LEFT:
			m_position -= m_rightDirection * velocity;
			break;
		case MOTION::RIGHT:
			m_position += m_rightDirection * velocity;
			break;
		default:
			break;
		}
	}

	void Camera::ProcessCursorPos(float const& xoffset, float const& yoffset, bool const constrainPitch) noexcept
	{
		float deltaX{ xoffset * m_sensitivity };
		float deltaY{ yoffset * m_sensitivity };

		m_yaw += deltaX;
		m_pitch += deltaY;

		if (constrainPitch)
		{
			if (m_pitch < -89.0f)
			{
				m_pitch = -89.0f;
			}
			else {}

			if (m_pitch > 89.0f)
			{
				m_pitch = 89.0f;
			}
			else {}
		}
		else {}

		Update();
	}

	void Camera::ProcessScroll(float const& yoffset) noexcept
	{
		m_zoom -= yoffset;

		if (m_zoom < 1.0f)
		{
			m_zoom = 1.0f;
		}
		else {}

		if (m_zoom > ZOOM)
		{
			m_zoom = ZOOM;
		}
		else {}
	}

	void Camera::Update() noexcept
	{
		glm::vec3 front{};
		front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		front.y = sin(glm::radians(m_pitch));
		front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
		m_frontDirection = glm::normalize(front);

		m_rightDirection = glm::normalize(glm::cross(m_frontDirection, m_worldUp));
		m_upDirection = glm::normalize(glm::cross(m_rightDirection, m_frontDirection));
	}

	void Camera::SetPosition(glm::vec3 const& position) noexcept
	{
		m_position = position;
	}
}