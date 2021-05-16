#include <GL3/PostProcessing.hpp>
#include <GL3/Shader.hpp>
#include <iostream>
#include <glad/glad.h>

namespace GL3 {

	PostProcessing::PostProcessing()
		: _fbo(0), _color(0), _depth(0)
	{
		//! Do nothing
	}

	PostProcessing::~PostProcessing()
	{
		CleanUp();
	}

	bool PostProcessing::Initialize()
	{
		glCreateFramebuffers(1, &_fbo);
		_debug.SetObjectName(GL_FRAMEBUFFER, _fbo, "PostProcessing FrameBuffer");

		glCreateTextures(GL_TEXTURE_2D, 1, &_color);
		glTextureParameteri(_color, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(_color, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(_color, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_color, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(_fbo, GL_COLOR_ATTACHMENT0, _color, 0);
		_debug.SetObjectName(GL_TEXTURE, _color, "PostProcessing Color Attachment");

		glCreateTextures(GL_TEXTURE_2D, 1, &_depth);
		glTextureParameteri(_depth, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(_depth, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(_depth, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_depth, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glNamedFramebufferTexture(_fbo, GL_DEPTH_ATTACHMENT, _depth, 0);
		_debug.SetObjectName(GL_TEXTURE, _depth, "PostProcessing Depth Attachment");

		glCreateVertexArrays(1, &_vao);
		_shader = std::make_unique< GL3::Shader >();
		if (!_shader->Initialize({ {GL_VERTEX_SHADER, RESOURCES_DIR "/shaders/quad.glsl"},
								   {GL_FRAGMENT_SHADER, RESOURCES_DIR "/shaders/postprocessing.glsl"} }))
		{
			DebugUtils::PrintStack();
			std::cerr << "[PostProcessing:Initialize] Failed to create postprocessing shader" << std::endl;
			return false;
		}

		_shader->BindShaderProgram();
		_shader->SendUniformVariable("color", 0);
		_shader->SendUniformVariable("depth", 1);
		_shader->BindFragDataLocation("fragColor", 0);

		return true;
	}

	void PostProcessing::Render() const
	{
		auto scope = _debug.ScopeLabel("Start PostProcessing");
		_shader->BindShaderProgram();
		glBindTextureUnit(0, _color);
		glBindTextureUnit(1, _depth);
		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void PostProcessing::Resize(const glm::ivec2& extent)
	{
		glTextureStorage2D(_color, 1, GL_RGB8, extent.x, extent.y);
		glTextureStorage2D(_depth, 1, GL_DEPTH_COMPONENT24, extent.x, extent.y);
	}

	void PostProcessing::CleanUp()
	{
		if (_depth) glDeleteTextures(1, &_depth);
		if (_color) glDeleteTextures(1, &_color);
		if (_fbo)	glDeleteFramebuffers(1, &_fbo);
	}

};