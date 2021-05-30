#include <GLTFSceneApp.hpp>
#include <GL3/Window.hpp>
#include <GL3/PerspectiveCamera.hpp>
#include <GL3/Shader.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <tinygltf/stb_image.h>

GLTFSceneApp::GLTFSceneApp()
{
	//! Do nothing
}

GLTFSceneApp::~GLTFSceneApp()
{
	//! Do nothing
}

bool GLTFSceneApp::OnInitialize(std::shared_ptr<GL3::Window> window)
{
	auto defaultCam = std::make_shared<GL3::PerspectiveCamera>();

	if (!defaultCam->SetupUniformBuffer())
		return false;

	defaultCam->SetupCamera(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	defaultCam->SetProperties(window->GetAspectRatio(), 60.0f, 0.1f, 100.0f);
	defaultCam->UpdateMatrix();

	AddCamera(std::move(defaultCam));

	auto defaultShader = std::make_shared<GL3::Shader>();
	if (!defaultShader->Initialize({ {GL_VERTEX_SHADER,	  RESOURCES_DIR "shaders/vertex.glsl"},
									 {GL_FRAGMENT_SHADER, RESOURCES_DIR "shaders/output.glsl"} }))
		return false;

	defaultShader->BindUniformBlock("UBOCamera", 0);

	auto skyboxShader = std::make_shared<GL3::Shader>();
	if (!skyboxShader->Initialize({ {GL_VERTEX_SHADER,	 RESOURCES_DIR "shaders/skybox.vert"},
									{GL_FRAGMENT_SHADER, RESOURCES_DIR "shaders/skybox.frag"}}))
		return false;

	_debug.SetObjectName(GL_PROGRAM, defaultShader->GetResourceID(), "Default Program");
	_debug.SetObjectName(GL_PROGRAM, skyboxShader->GetResourceID(), "Skybox Program");
	_shaders.emplace("default", std::move(defaultShader));
	_shaders.emplace("skybox", std::move(skyboxShader));

	/*if (!_sceneInstance.Initialize(configure["scene"].as<std::string>(),
		Core::VertexFormat::Position3Normal3TexCoord2Color4))
		return false;

	if (!_skyDome.Initialize(configure["envmap"].as<std::string>()))
		return false;*/

	glGenBuffers(1, &_uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneData), &_sceneData, GL_STATIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	return true;
}

void GLTFSceneApp::OnCleanUp()
{
	_sceneInstance.CleanUp();
}

void GLTFSceneApp::OnUpdate(double dt)
{
	(void)dt;
}

void GLTFSceneApp::OnDraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.8f, 1.0f);

	const auto& iblTextures = _skyDome.GetIBLTextureSet();

	auto& skyboxShader = _shaders["skybox"];
	skyboxShader->BindShaderProgram();

	_cameras[0]->BindCamera(0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uniformBuffer);
	_skyDome.Render(skyboxShader, GL_BLEND_SRC_ALPHA);

	auto& pbrShader = _shaders["default"];
	pbrShader->BindShaderProgram();
	glBindTextureUnit(0, iblTextures.irradianceCube);
	glBindTextureUnit(1, iblTextures.brdfLUT);
	glBindTextureUnit(2, iblTextures.prefilteredCube);

	_cameras[0]->BindCamera(0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, _uniformBuffer);
	_sceneInstance.Render(pbrShader, GL_BLEND_SRC_ALPHA);
}

void GLTFSceneApp::OnProcessInput(unsigned int key)
{
	if (key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
	{
		_sceneData.materialMode = static_cast<int>(key - GLFW_KEY_1);
		//! Update uniform buffer
		glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
		glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SceneData, materialMode), sizeof(int), &_sceneData.materialMode);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}

void GLTFSceneApp::OnProcessResize(int width, int height)
{
	(void)width; (void)height;
}
