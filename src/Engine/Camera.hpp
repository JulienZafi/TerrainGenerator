#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

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
		Camera() = default;
		~Camera() = default;

		/**
		* Singletons should not be cloneable.
		*/
		Camera(Camera& camera) = delete;
		/**
		 * Singletons should not be assignable.
		 */
		void operator=(const Camera&) = delete;
		/**
		 * This is the static method that controls the access to the singleton
		 * instance. On the first run, it creates a singleton object and places it
		 * into the static field. On subsequent runs, it returns the client existing
		 * object stored in the static field.
		 */
		static std::unique_ptr <Camera>& GetInstance();

		static void Move(glm::vec3 const &position) noexcept;

		static void InvertPitch() noexcept;

		/*
		* PROCESS USER INPUTS
		*/
		static void ProcessKeyboard(MOTION const motion, float const &deltaTime) noexcept;
		static void ProcessCursorPos(float const& xoffset, float const& yoffset, bool const constrainPitch = true) noexcept;
		static void ProcessScroll(float const& yoffset) noexcept;

		/*
		* GETTERS
		*/
		[[nodiscard]] inline const glm::vec3 Position() const noexcept { return position_; }
		[[nodiscard]] inline const glm::vec3 FrontDirection() const noexcept { return frontDirection_; }
		[[nodiscard]] inline const float Zoom() const noexcept { return zoom_; }
		[[nodiscard]] inline const glm::mat4 ViewMatrix() const noexcept { return glm::lookAt(position_, position_ + frontDirection_, upDirection_); }

		/*
		* SETTERS
		*/
		static void UpdateHeight(float const& height) noexcept;

	private:
		/*
		* POSITIONS & DIRECTIONS
		*/		
		static glm::vec3 frontDirection_;
		static glm::vec3 upDirection_;
		static glm::vec3 rightDirection_;
		static glm::vec3 worldUp_;

		/*
		* EULER ANGLES
		*/
		static float yaw_;
		static float pitch_;

		/*
		* SINGLETON INSTANCE
		*/
		static std::unique_ptr <Camera> camera_;

		static glm::vec3 position_;

		static float zoom_;
	};
}