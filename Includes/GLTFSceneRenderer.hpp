#ifndef GLTF_SCENE_RENDERER_HPP
#define GLTF_SCENE_RENDERER_HPP

#include <GL3/Renderer.hpp>

class GLTFSceneRenderer : public GL3::Renderer
{
public:
	//! Default constructor
	GLTFSceneRenderer();
	//! Default desctrutor
	~GLTFSceneRenderer();
protected:
	bool OnInitialize(const cxxopts::ParseResult& configure) override;
	void OnCleanUp() override;
	void OnUpdateFrame(double dt) override;
	void OnBeginDraw() override;
	void OnEndDraw() override;
	void OnProcessInput(unsigned int key) override;
	void OnProcessResize(int width, int height) override;
};

#endif //! end of GLTFSceneRenderer.hpp