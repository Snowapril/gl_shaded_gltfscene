#include <Application.hpp>
#include <GL3/Camera.hpp>
#include <GL3/PerspectiveCamera.hpp>
#include <GL3/DebugUtils.hpp>
#include <GL3/Shader.hpp>
#include <GL3/Window.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

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
	if (!OnWindowInit(configure))
		return false;

	if (!OnCameraInit(configure))
		return false;

	if (!OnShaderInit(configure))
		return false;

	if (!OnTextureInit(configure))
		return false;

	return true;
}

bool Application::OnWindowInit(const cxxopts::ParseResult& configure)
{
	(void)configure;
	if (!AddWindow("modern-opengl-template", 1200, 900))
		return false;

	return true;
}

bool Application::OnCameraInit(const cxxopts::ParseResult& configure)
{
	(void)configure;
	auto defaultCam = std::make_shared<GL3::PerspectiveCamera>();
	
	if (!defaultCam->SetupUniformBuffer())
		return false;

	defaultCam->SetupCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	defaultCam->SetProperties(_currentWindow.lock()->GetAspectRatio(), 60.0f, 0.1f, 100.0f);
	defaultCam->UpdateMatrix();

	AddCamera(std::move(defaultCam));

	return true;
}

bool Application::OnShaderInit(const cxxopts::ParseResult& configure)
{
	(void)configure;
	auto defaultShader = std::make_shared<GL3::Shader>();
	if (!defaultShader->Initialize({ {GL_VERTEX_SHADER, RESOURCES_DIR "/shaders/vertex.glsl"},
									 {GL_FRAGMENT_SHADER, RESOURCES_DIR "/shaders/output.glsl"} }))
		return false;

	defaultShader->BindUniformBlock("CamMatrices", 0);
	_shaders.emplace("default", std::move(defaultShader));

	return true;
}

bool Application::OnTextureInit(const cxxopts::ParseResult& configure)
{
	(void)configure;
	stbi_set_flip_vertically_on_load(true);

	/*int width, height, numChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &numChannels);

	if (data == nullptr || width == 0 || height == 0 || numChannels == 0)
	{
		std::cerr << "Failed to open image " << path << std::endl;
		GL3::StackTrace::PrintStack();
		return false;
	}

	stbi_image_free(data);*/

	return true;
}

bool Application::AddWindow(const std::string& title, int width, int height)
{
	//! Create window shared_ptr with default constructor
	auto window = std::make_shared<GL3::Window>();

	//! Initialize the window and check the returned error.
	if (!window->Initialize(title, width, height))
		return false;

	//! Pass the Application input handling method to window.
	using namespace std::placeholders;
	std::function<void(unsigned int)> inputCallback = std::bind(&Application::ProcessInput, this, std::placeholders::_1);
	std::function<void(double, double)> cursorCallback = std::bind(&Application::ProcessCursorPos, this, std::placeholders::_1, std::placeholders::_2);
	window->operator+=(inputCallback);
	window->operator+=(cursorCallback);

	//! Assign window smart pointer to weak_ptr
	_currentWindow = window;
	_windows.push_back(window);

	return true;
}

void Application::AddCamera(std::shared_ptr<GL3::Camera>&& camera)
{
	_cameras.emplace_back(std::move(camera));
}

void Application::UpdateFrame(double dt)
{
	(void)dt;
}

void Application::DrawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.9f, 1.0f);
}

void Application::CleanUp()
{
	_currentWindow.reset();
}

std::shared_ptr<GL3::Window> Application::GetCurrentWindow() const
{
	return _currentWindow.expired() ? nullptr : _currentWindow.lock();
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