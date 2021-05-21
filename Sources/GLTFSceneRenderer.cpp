#include <GLTFSceneRenderer.hpp>
#include <GLTFSceneApp.hpp>
#include <glad/glad.h>

GLTFSceneRenderer::GLTFSceneRenderer()
{
	//! Do nothing
}

GLTFSceneRenderer::~GLTFSceneRenderer()
{
	//! Do nothing
}

bool GLTFSceneRenderer::OnInitialize(const cxxopts::ParseResult& configure)
{
	if (!AddApplication(std::make_shared<GLTFSceneApp>(), configure))
		return false;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void GLTFSceneRenderer::OnCleanUp()
{
	//! Do nothing
}

void GLTFSceneRenderer::OnUpdateFrame(double dt)
{
	(void)dt;
}

void GLTFSceneRenderer::OnBeginDraw()
{
	//! Do nothing
}

void GLTFSceneRenderer::OnEndDraw()
{
	//! Do nothing
}

void GLTFSceneRenderer::OnProcessInput(unsigned int key)
{
	(void)key;
}

void GLTFSceneRenderer::OnProcessResize(int width, int height)
{
	(void)width; (void)height;
}