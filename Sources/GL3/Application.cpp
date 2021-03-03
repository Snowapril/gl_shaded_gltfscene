#include <GL3/Application.hpp>
#include <GL3/Window.hpp>
#include <GL3/Camera.hpp>
#include <GL3/PerspectiveCamera.hpp>
#include <GL3/DebugUtils.hpp>
#include <GL3/Shader.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

namespace GL3 {

	Application::Application()
	{
		//! Do nothing
	}

	Application::~Application()
	{
		CleanUp();
	}

	bool Application::Initialize(const cxxopts::ParseResult& configure)
	{
		//! initialize here
		
		
		if (!OnInitialize(configure))
			return false;

		return true;
	}

	bool Application::InitWindow(const std::string& title, int width, int height)
	{
		//! Create window shared_ptr with default constructor
		_window = std::make_shared<Window>();

		//! Initialize the window and check the returned error.
		if (!_window->Initialize(title, width, height))
			return false;

		//! Pass the Application input handling method to window.
		using namespace std::placeholders;
		std::function<void(unsigned int)> inputCallback = std::bind(&Application::ProcessInput, this, std::placeholders::_1);
		std::function<void(double, double)> cursorCallback = std::bind(&Application::ProcessCursorPos, this, std::placeholders::_1, std::placeholders::_2);
		_window->operator+=(inputCallback);
		_window->operator+=(cursorCallback);

		return true;
	}

	void Application::AddCamera(std::shared_ptr<Camera>&& camera)
	{
		_cameras.emplace_back(std::move(camera));
	}

	void Application::Update(double dt)
	{
		OnUpdate(dt);
	}

	void Application::Draw()
	{
		OnDraw();
	}

	void Application::CleanUp()
	{
		_window.reset();

		OnCleanUp();
	}

	std::shared_ptr<Window> Application::GetWindow() const
	{
		return _window;
	}

	void Application::ProcessInput(unsigned int key)
	{
		for (auto& camera : _cameras)
			camera->ProcessInput(key);
	}

	void Application::ProcessCursorPos(double xpos, double ypos)
	{
		for (auto& camera : _cameras)
			camera->ProcessCursorPos(xpos, ypos);
	}

};