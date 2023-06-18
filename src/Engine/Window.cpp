
#include "Window.hpp"
#include "Camera.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace Engine
{
	/*
	* INITIALIZE STATIC VARIABLES
	*/
	unsigned int Window::width = 0;
	unsigned int Window::height = 0;  
	bool Window::useWireFrame = false;
	float Window::deltaTime = 0.0f;
	float Window::lastFrame = 0.0f;
	bool Window::isFirstMouse = true;
	float Window::lastXpos = 0.0f;
	float Window::lastYpos = 0.0f;
	Keyboard Window::keys = Keyboard();

	Window::Window(unsigned int const width, unsigned int const height, std::string_view const& title) noexcept
	{
		this->width = width;
		this->height = height;

		Create(title);
	}

	void Window::Create(std::string_view const& title) noexcept
	{
		if (!glfwInit())
		{
			std::cout << "ERROR::GLFW::WINDOW::FAILED_TO_INITIALIZE!!!\n";
			return;
		}
		else{}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ENGINE_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ENGINE_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		m_window = glfwCreateWindow((int)width, (int)height, title.data(), glfwGetPrimaryMonitor(), nullptr);

		if (!m_window)
		{
			std::cout << "ERROR::GLFW::WINDOW::FAILED_TO_CREATE!!!\n";
		}
		else{}

		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "ERROR::GLAD::WINDOWFAILED_TO_INITIALIZE!!!\n";
		}
		else{}

		glfwSetFramebufferSizeCallback(m_window, framebuffersize_callback);
		glfwSetCursorPosCallback(m_window, cursorpos_callback);
		glfwSetScrollCallback(m_window, scroll_callback);
		glfwSetKeyCallback(m_window, key_callback);

		glEnable(GL_DEPTH_TEST);
	}

	void Window::ProcessInput() noexcept
	{
		if (keys.is_Esc_pressed)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		else{}

		/*
		* CAMERA MOTION
		*/
		if (keys.is_W_pressed)
		{
			Camera::ProcessKeyboard(MOTION::FORWARD, deltaTime);
		}
		else {}

		if (keys.is_S_pressed)
		{
			Camera::ProcessKeyboard(MOTION::BACKWARD, deltaTime);
		}
		else {}

		if (keys.is_A_pressed)
		{
			Camera::ProcessKeyboard(MOTION::LEFT, deltaTime);
		}
		else {}

		if (keys.is_D_pressed)
		{
			Camera::ProcessKeyboard(MOTION::RIGHT, deltaTime);
		}
		else {}

		if (keys.is_Space_pressed)
		{
			useWireFrame = !useWireFrame;
			if (useWireFrame)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
		else {}
	}

	bool Window::ShouldClose() const noexcept
	{
		return glfwWindowShouldClose(m_window);
	}

	void Window::Close() const noexcept
	{
		glfwTerminate();
	}

	void Window::UpdateTime() noexcept
	{
		float currentFrame{ (float)glfwGetTime() };
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	void Window::Wait() const noexcept
	{
		glfwSwapBuffers(m_window);
		glfwWaitEvents();
	}

	/*
	* WINDOW CALLBACKS
	*/
	void Window::framebuffersize_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Window::cursorpos_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (isFirstMouse)
		{
			lastXpos = (float)xpos;
			lastYpos = (float)ypos;
			isFirstMouse = false;
		}
		else{}

		float xoffset{ (float)xpos - lastXpos };
		float yoffset{ lastYpos - (float)ypos };

		lastXpos = (float)xpos;
		lastYpos = (float)ypos;

		Camera::ProcessCursorPos(xoffset, yoffset);
	}

	void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		Camera::ProcessScroll((float)yoffset);
	}

	void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS) 
		{

			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				keys.is_Esc_pressed = true;
				break;
			case GLFW_KEY_W:
				keys.is_W_pressed = true;
				break;
			case GLFW_KEY_S:
				keys.is_S_pressed = true;
				break;
			case GLFW_KEY_A:
				keys.is_A_pressed = true;
				break;
			case GLFW_KEY_D:
				keys.is_D_pressed = true;
				break;
			case GLFW_KEY_SPACE:
				keys.is_Space_pressed = true;
				break;
			default:
				break;
			}
		}
		else if (action == GLFW_RELEASE)
		{
			switch (key)
			{
			case GLFW_KEY_ESCAPE:
				keys.is_Esc_pressed = false;
				break;
			case GLFW_KEY_W:
				keys.is_W_pressed = false;
				break;
			case GLFW_KEY_S:
				keys.is_S_pressed = false;
				break;
			case GLFW_KEY_A:
				keys.is_A_pressed = false;
				break;
			case GLFW_KEY_D:
				keys.is_D_pressed = false;
				break;
			case GLFW_KEY_SPACE:
				keys.is_Space_pressed = false;
				break;
			default:
				break;
			}
		}
		else{}
	}
}