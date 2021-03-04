#include <SampleRenderer.hpp>
#include <SampleApp.hpp>

SampleRenderer::SampleRenderer()
{
	//! Do nothing
}

SampleRenderer::~SampleRenderer()
{
	//! Do nothing
}

bool SampleRenderer::OnInitialize(const cxxopts::ParseResult& configure)
{
	if (!AddApplication(std::make_shared<SampleApp>(), configure))
		return false;
	if (!AddApplication(std::make_shared<SampleApp>(), configure))
		return false;
	return true;
}

void SampleRenderer::OnCleanUp()
{
	//! Do nothing
}

void SampleRenderer::OnUpdateFrame(double dt)
{
	(void)dt;
}

void SampleRenderer::OnBeginDraw()
{
	//! Do nothing
}

void SampleRenderer::OnEndDraw()
{
	//! Do nothing
}

void SampleRenderer::OnProcessInput(unsigned int key)
{
	(void)key;
}