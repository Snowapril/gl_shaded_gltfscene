#ifndef SAMPLE_RENDERER_HPP
#define SAMPLE_RENDERER_HPP

#include <GL3/Renderer.hpp>

class SampleRenderer : public GL3::Renderer
{
public:
	//! Default constructor
	SampleRenderer();
	//! Default desctrutor
	~SampleRenderer();
protected:
	bool OnInitialize(const cxxopts::ParseResult& configure) override;
	void OnCleanUp() override;
	void OnUpdateFrame(double dt) override;
	void OnBeginDraw() override;
	void OnEndDraw() override;
	void OnProcessInput(unsigned int key) override;
};

#endif //! end of SampleRenderer.hpp