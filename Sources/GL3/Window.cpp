#include <GL3/Window.hpp>
#include <GL3/DebugUtils.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

namespace
{
	std::vector<GL3::Window*> gWindowPtrs;

	GL3::Window* GetMatchedWindow(GLFWwindow* window)
	{
		for (auto windowPtr : gWindowPtrs)
			if (window == windowPtr->GetGLFWWindow())
				return windowPtr;
		return nullptr;
	}

	void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		GetMatchedWindow(window)->ProcessCursorPos(xpos, ypos);
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
		for (auto iter = gWindowPtrs.begin(); iter != gWindowPtrs.end();)
		{
			if (this == *iter)
				iter = gWindowPtrs.erase(iter);
			else
				++iter;
		}
		CleanUp();
	}

	bool Window::Initialize(const std::string& title, int width, int height, GLFWwindow* sharedWindow)
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

		this->_window = glfwCreateWindow(width, height, title.c_str(), nullptr, sharedWindow);
		
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
		//! If there is no more glfw window, cleanup context too.
		if (gWindowPtrs.empty())
			glfwTerminate();
	}

	GLFWwindow* Window::GetGLFWWindow()
	{
		return this->_window;
	}

	glm::ivec2 Window::GetWindowExtent() const
	{
		return this->_windowExtent;
	}

	//! Process Input
	void Window::ProcessInput() const
	{
		for (unsigned int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
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