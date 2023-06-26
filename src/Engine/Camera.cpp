#include "Camera.hpp"

namespace Engine
{
	std::unique_ptr <Camera> Camera::camera_ = nullptr;
	glm::vec3 Camera::position_ = glm::vec3(0.0f, 0.0f, 0.0f);
	float Camera::zoom_ = ZOOM;
	glm::vec3 Camera::frontDirection_ = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Camera::upDirection_ = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 Camera::rightDirection_ = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 Camera::worldUp_ = upDirection_;
	float Camera::yaw_ = YAW;
	float Camera::pitch_ = PITCH;

	/**
	 * Static methods should be defined outside the class.
	 */
	std::unique_ptr <Camera>& Camera::GetInstance()
	{
		/**
		 * This is a safer way to create an instance. instance = new Singleton is
		 * dangeruous in case two instance threads wants to access at the same time
		 */
		if (camera_ == nullptr) 
		{
			camera_ = std::make_unique <Camera>();

			// Update vectors
			glm::vec3 front{};
			front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
			front.y = sin(glm::radians(pitch_));
			front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
			frontDirection_ = glm::normalize(front);

			rightDirection_ = glm::normalize(glm::cross(frontDirection_, worldUp_));
			upDirection_ = glm::normalize(glm::cross(rightDirection_, frontDirection_));
		}
		else{}
		
		return camera_;
	}

	void Camera::MirrorY() noexcept
	{
		position_.y *= -1;
	}

	void Camera::Move(glm::vec3 const &position) noexcept
	{
		position_ += position;
	}

	void Camera::InvertPitch() noexcept
	{
		pitch_ = pitch_;
	}

	void Camera::ProcessKeyboard(MOTION const motion, float const& deltaTime) noexcept
	{
		float velocity{ deltaTime * SPEED };
		switch (motion)
		{
		case MOTION::FORWARD:
			position_ += frontDirection_ * velocity;
			break;
		case MOTION::BACKWARD:
			position_ -= frontDirection_ * velocity;
			break;
		case MOTION::LEFT:
			position_ -= rightDirection_ * velocity;
			break;
		case MOTION::RIGHT:
			position_ += rightDirection_ * velocity;
			break;
		default:
			break;
		}
	}

	void Camera::ProcessCursorPos(float const& xoffset, float const& yoffset, bool const constrainPitch) noexcept
	{
		float deltaX{ xoffset * SENSITIVITY };
		float deltaY{ yoffset * SENSITIVITY };

		yaw_ += deltaX;
		pitch_ += deltaY;

		if (constrainPitch)
		{
			if (pitch_ < -89.0f)
			{
				pitch_ = -89.0f;
			}
			else {}

			if (pitch_ > 89.0f)
			{
				pitch_ = 89.0f;
			}
			else {}
		}
		else {}

		// Update vectors
		glm::vec3 front{};
		front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		front.y = sin(glm::radians(pitch_));
		front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		frontDirection_ = glm::normalize(front);

		rightDirection_ = glm::normalize(glm::cross(frontDirection_, worldUp_));
		upDirection_ = glm::normalize(glm::cross(rightDirection_, frontDirection_));
	}

	void Camera::ProcessScroll(float const& yoffset) noexcept
	{
		zoom_ -= yoffset;

		if (zoom_ < 1.0f)
		{
			zoom_ = 1.0f;
		}
		else {}

		if (zoom_ > ZOOM)
		{
			zoom_ = ZOOM;
		}
		else {}
	}

	void Camera::UpdateHeight(float const& height) noexcept
	{
		position_.y = height;
	}
}