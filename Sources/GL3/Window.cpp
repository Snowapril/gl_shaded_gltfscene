#include <GL3/Window.hpp>
#include <GL3/DebugUtils.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

namespace
{
	std::vector<GL3::Window*> gWindowPtrs;

	GL3::Window* GetMatchedWinodw(GLFWwindow* window)
	{
		for (auto windowPtr : gWindowPtrs)
			if (window == windowPtr->GetWindow())
				return windowPtr;
		return nullptr;
	}

	void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		GetMatchedWinodw(window)->ProcessCursorPos(xpos, ypos);
	}
};


namespace GL3
{
	Window::Window()
		: _window(nullptr), _windowExtent(0, 0)
	{
		gWindowPtrs.push_back(this);
	}

	Window::Window(const std::string& title, int width, int height)
	{
		gWindowPtrs.push_back(this);
		Initialize(title, width, height);
	}

	Window::~Window()
	{
		CleanUp();
		for (auto iter = gWindowPtrs.begin(); iter != gWindowPtrs.end();)
		{
			if (this == *iter)
				iter = gWindowPtrs.erase(iter);
			else
				++iter;
		}
	}

	bool Window::Initialize(const std::string& title, int width, int height)
	{
		this->_windowTitle = title;
		this->_windowExtent = glm::ivec2(width, height);

		if (glfwInit() == GLFW_FALSE)
		{
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

		this->_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
		
		if (this->_window == nullptr)
		{
			std::cerr << "Failed to Create GLFW Window" << std::endl;
			StackTrace::PrintStack();
			return false;
		}

		glfwMakeContextCurrent(this->_window);

		if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == false)
		{
			std::cerr << "Failed to initialize GLAD" << std::endl;
			StackTrace::PrintStack();
			return false;
		}

		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(GL3Debug::DebugLog, nullptr);
		glDebugMessageControl(GL_DEBUG_SOURCE_APPLICATION, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
		glDebugMessageControl(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, NULL, false);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, NULL, false);
		
		glfwSetInputMode(this->_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwSetCursorPosCallback(this->_window, ::CursorPosCallback);

		return this->_window != nullptr;
	}

	void Window::CleanUp()
	{
		if (this->_window)
			glfwDestroyWindow(this->_window);
		glfwTerminate();
	}

	const GLFWwindow* Window::GetWindow() const
	{
		return this->_window;
	}

	bool Window::CheckWindowShouldClose() const
	{
		return glfwWindowShouldClose(this->_window) == GLFW_TRUE;
	}
	//! Poll window events
	void Window::PollEvents() const
	{
		glfwPollEvents();
	}
	//! Swap buffers
	void Window::SwapBuffer() const
	{
		glfwSwapBuffers(this->_window);
	}

	//! Process Input
	void Window::ProcessInput() const
	{
		static const std::vector<unsigned int> watchingKeys {
			GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, 
			GLFW_KEY_ESCAPE, GLFW_KEY_SPACE, GLFW_KEY_ENTER
		};

		for (auto key : watchingKeys)
			if (glfwGetKey(_window, key) == GLFW_PRESS)
			{
				for (auto& callback : _keyCallbacks)
					callback(key);
			}
	}

	void Window::ProcessCursorPos(double xpos, double ypos) const
	{
		for (auto& callback : _cursorPosCallbacks)
			callback(xpos, ypos);
	}

	void Window::operator+=(const KeyCallback& callback)
	{
		_keyCallbacks.push_back(callback);
	}

	void Window::operator+=(const CursorPosCallback& callback)
	{
		_cursorPosCallbacks.push_back(callback);
	}
	
	float Window::GetAspectRatio() const
	{
		return static_cast<float>(_windowExtent.x) / static_cast<float>(_windowExtent.y);
	}
};