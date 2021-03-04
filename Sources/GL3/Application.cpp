#include <GL3/Application.hpp>
#include <GL3/Camera.hpp>
#include <GL3/PerspectiveCamera.hpp>
#include <GL3/DebugUtils.hpp>
#include <GL3/Shader.hpp>
#include <GL3/Window.hpp>
#include <glad/glad.h>
#include <iostream>

namespace GL3 {

	Application::Application()
	{
		//! Do nothing
	}

	Application::~Application()
	{
		//! Do nothing
	}

	bool Application::Initialize(std::shared_ptr<GL3::Window> window, const cxxopts::ParseResult& configure)
	{
		if (!OnInitialize(window, configure))
			return false;

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
		_shaders.clear();
		_textures.clear();
		_cameras.clear();

		OnCleanUp();
	}

	void Application::ProcessInput(unsigned int key)
	{
		for (auto& camera : _cameras)
			camera->ProcessInput(key);

		OnProcessInput(key);
	}

	void Application::ProcessCursorPos(double xpos, double ypos)
	{
		for (auto& camera : _cameras)
			camera->ProcessCursorPos(xpos, ypos);
	}

};