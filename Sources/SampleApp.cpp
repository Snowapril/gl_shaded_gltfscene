#include <SampleApp.hpp>
#include <GL3/Window.hpp>
#include <GL3/PerspectiveCamera.hpp>
#include <GL3/Shader.hpp>
#include <GL3/Texture.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tinygltf/stb_image.h>

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
	auto defaultCam = std::make_shared<GL3::PerspectiveCamera>();

	if (!defaultCam->SetupUniformBuffer())
		return false;

	defaultCam->SetupCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	defaultCam->SetProperties(window->GetAspectRatio(), 60.0f, 0.1f, 100.0f);
	defaultCam->UpdateMatrix();

	AddCamera(std::move(defaultCam));

	auto defaultShader = std::make_shared<GL3::Shader>();
	if (!defaultShader->Initialize({ {GL_VERTEX_SHADER, RESOURCES_DIR "shaders/vertex.glsl"},
									 {GL_FRAGMENT_SHADER, RESOURCES_DIR "shaders/output.glsl"} }))
		return false;

	defaultShader->BindUniformBlock("UBOCamera", 0);
	_shaders.emplace("default", std::move(defaultShader));

	stbi_set_flip_vertically_on_load(true);

	if (!_sceneInstance.Initialize(RESOURCES_DIR + configure["scene"].as<std::string>(),
		Core::VertexFormat::Position3Normal3TexCoord2Color4))
		return false;

	return true;
}

void SampleApp::OnCleanUp()
{
	_sceneInstance.CleanUp();
}

void SampleApp::OnUpdate(double dt)
{
	(void)dt;
}

void SampleApp::OnDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);

	_shaders["default"]->BindShaderProgram();
	_cameras[0]->BindCamera(0);
	_sceneInstance.Render(_shaders["default"], GL_BLEND_SRC_ALPHA);
}

void SampleApp::OnProcessInput(unsigned int key)
{
	(void)key;
}

void SampleApp::OnProcessResize(int width, int height)
{
	(void)width; (void)height;
}
