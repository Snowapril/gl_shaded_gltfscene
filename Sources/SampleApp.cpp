#include <SampleApp.hpp>
#include <GL3/Window.hpp>
#include <GL3/PerspectiveCamera.hpp>
#include <GL3/Shader.hpp>
#include <GL3/Texture.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

SampleApp::SampleApp()
{
	//! Do nothing
}

SampleApp::~SampleApp()
{
	//! Do nothing
}

bool SampleApp::OnInitialize(std::shared_ptr<GL3::Window> window, const cxxopts::ParseResult& configure)
{
	(void)configure;

	auto defaultCam = std::make_shared<GL3::PerspectiveCamera>();

	if (!defaultCam->SetupUniformBuffer())
		return false;

	defaultCam->SetupCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	defaultCam->SetProperties(window->GetAspectRatio(), 60.0f, 0.1f, 100.0f);
	defaultCam->UpdateMatrix();

	AddCamera(std::move(defaultCam));

	auto defaultShader = std::make_shared<GL3::Shader>();
	if (!defaultShader->Initialize({ {GL_VERTEX_SHADER, RESOURCES_DIR "/shaders/vertex.glsl"},
									 {GL_FRAGMENT_SHADER, RESOURCES_DIR "/shaders/output.glsl"} }))
		return false;

	defaultShader->BindUniformBlock("CamMatrices", 0);
	_shaders.emplace("default", std::move(defaultShader));

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

void SampleApp::OnCleanUp()
{
	//! Do nothing
}

void SampleApp::OnUpdate(double dt)
{
	(void)dt;
}

void SampleApp::OnDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);
}

void SampleApp::OnProcessInput(unsigned int key)
{
	(void)key;
}