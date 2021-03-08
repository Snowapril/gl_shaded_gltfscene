#include <GL3/PostProcessing.hpp>
#include <glad/glad.h>

namespace GL3 {

	PostProcessing::PostProcessing()
		: _fbo(0), _rbo(0), _color(0), _depth(0)
	{
		//! Do nothing
	}

	PostProcessing::~PostProcessing()
	{
		CleanUp();
	}

	bool PostProcessing::Initialize()
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenRenderbuffers(1, &_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

		glGenTextures(1, &_color);
		glBindTexture(GL_TEXTURE_2D, _color);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _color, 0);

		glGenTextures(1, &_depth);
		glBindTexture(GL_TEXTURE_2D, _depth);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &_vao);

		_shader = std::make_unique< GL3::Shader >();
		if (!_shader->Initialize({ {GL_VERTEX_SHADER, RESOURCES_DIR "/shaders/quad.glsl"},
								   {GL_FRAGMENT_SHADER, RESOURCES_DIR "/shaders/postprocessing.glsl"}}))
			return false;
		_shader->BindShaderProgram();
		_shader->SendUniformVariable("color", 0);
		_shader->SendUniformVariable("depth", 1);

		return true;
	}

	void PostProcessing::Render() const
	{
		_shader->BindShaderProgram();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _color);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _depth);
		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	void PostProcessing::Resize(const glm::ivec2& extent)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, extent.x, extent.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, _color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, extent.x, extent.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glBindTexture(GL_TEXTURE_2D, _depth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, extent.x, extent.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void PostProcessing::CleanUp()
	{
		if (_depth) glDeleteTextures(1, &_depth);
		if (_color) glDeleteTextures(1, &_color);
		if (_rbo) glDeleteRenderbuffers(1, &_rbo);
		if (_fbo) glDeleteFramebuffers(1, &_fbo);
	}

};