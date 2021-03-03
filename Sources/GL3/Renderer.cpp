#include <GL3/Renderer.hpp>
#include <GL3/Application.hpp>
#include <GL3/Camera.hpp>
#include <GL3/Window.hpp>
#include <glad/glad.h>

namespace GL3 {

	Renderer::Renderer()
	{
		//! Do nothing
	}

	Renderer::~Renderer()
	{
		CleanUp();
	}

	bool Renderer::Initialize(const cxxopts::ParseResult& configure)
	{
		if (!OnInitialize(configure))
			return false;

		return true;
	}

	bool Renderer::AddApplication(std::shared_ptr<Application> app, const cxxopts::ParseResult& configure)
	{
		if (_applications.empty())
			_currentApp = app;

		_applications.push_back(app);

		return app->Initialize(configure);
	}

	void Renderer::UpdateFrame(double dt)
	{
		OnUpdateFrame(dt);
	}

	void Renderer::DrawFrame()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.9f, 1.0f);

		OnDrawFrame();
	}

	void Renderer::CleanUp()
	{
		_currentApp.reset();

		OnCleanUp();
	}

	std::shared_ptr<GL3::Application> Renderer::GetCurrentApplication() const
	{
		return _currentApp.expired() ? nullptr : _currentApp.lock();
	}
	
	std::shared_ptr< GL3::Window > Renderer::GetCurrentWindow() const
	{
		auto app = GetCurrentApplication();
		return app ? app->GetWindow() : nullptr;
	}

	void Renderer::ProcessInput(unsigned int key)
	{
		OnProcessInput(key);
	}

	bool Renderer::OnInitialize(const cxxopts::ParseResult& configure)
	{
		(void)configure;
		//! Do nothing
		return true;
	}

	void Renderer::OnCleanUp()
	{
		//! Do nothing
	}

	void Renderer::OnUpdateFrame(double dt)
	{
		(void)dt;
		//! Do nothing
	}

	void Renderer::OnDrawFrame()
	{
		//! Do nothing
	}

	void Renderer::OnProcessInput(unsigned int key)
	{
		(void)key;
		//! Do nothing
	}
};