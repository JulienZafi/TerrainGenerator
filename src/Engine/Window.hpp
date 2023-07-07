#pragma once

#ifndef IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#endif

#include "Camera.hpp"

#include <memory>
#include <string_view>

struct GLFWwindow;

namespace Engine
{
#define ENGINE_MAJOR_VERSION 4
#define ENGINE_MINOR_VERSION 1

	struct Keyboard
	{
		bool is_Esc_pressed;
		bool is_W_pressed;
		bool is_S_pressed;
		bool is_A_pressed;
		bool is_D_pressed;
		bool is_Space_pressed;
	};

	class Window
	{
	public:
		Window() = delete;
		Window(Window const&) = delete;
		~Window() = default;

		Window(unsigned int const defaultWidth = 800, unsigned int const defaultHeight = 600, std::string_view const& title = "Window") noexcept;

		void MakeCurrent() const noexcept;

		/*
		* UI
		*/
		void InitGUI() const;
		void startImGUIFrame() const;
		void DrawGUI() const;

		/*
		* UPDATE
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
		[[nodiscard]] inline const float DeltaTime() const noexcept { return deltaTime; }

	private:
		static unsigned int width;
		static unsigned int height;
		GLFWwindow *m_window = nullptr;

		void Create(std::string_view const& title) noexcept;

		/*
		* WINDOW CALLBACKS
		*/
		static void framebuffersize_callback(GLFWwindow* window, int width, int height);
		static void cursorpos_callback(GLFWwindow* window, double xpos, double ypos);
		static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
		static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		
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

		/*
		* KEYBOARD INPUTS
		*/
		static Keyboard keys;
		static bool useWireFrame;
	};
}