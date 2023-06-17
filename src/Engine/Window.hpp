#pragma once

#include "Camera.hpp"

#include <memory>
#include <string_view>

struct GLFWwindow;

namespace Engine
{
#define ENGINE_MAJOR_VERSION 4
#define ENGINE_MINOR_VERSION 1

	class Window
	{
	public:
		Window() = delete;
		Window(Window const&) = delete;
		~Window() = default;

		Window(unsigned int const width = 800, unsigned int const height = 600, std::string_view const& title = "Window") noexcept;

		/*
		* UPDATE CURRENT DELTATIME
		*/
		void UpdateTime() noexcept;

		/*
		* USER INPUTS
		*/
		void ProcessInput() noexcept;
		void Wait() const noexcept;

		/*
		* RELEASE RESOURCES
		*/
		void Close() const noexcept;
		bool ShouldClose() const noexcept;

		/*
		* GETTERS
		*/
		[[nodiscard]] inline const unsigned int Width() const noexcept { return width; }
		[[nodiscard]] inline const unsigned int Height() const noexcept { return height; }
		[[nodiscard]] inline const GLFWwindow* GetInstance() const noexcept { return m_window; }

	private:
		static unsigned int width;
		static unsigned int height;

		static bool useWireFrame;

		GLFWwindow *m_window = nullptr;

		void Create(std::string_view const& title) noexcept;

		/*
		* WINDOW CALLBACKS
		*/
		static void framebuffersize_callback(GLFWwindow* window, int width, int height);
		static void cursorpos_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		
		/*
		* TIMING
		*/
		static float deltaTime;
		static float lastFrame;

		/*
		* CURSOR POS
		*/
		static float lastXpos;
		static float lastYpos;
		static bool isFirstMouse;
	};
}