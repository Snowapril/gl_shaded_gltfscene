#include <GL3/Texture.hpp>
#include <GL3/DebugUtils.hpp>
#include <glad/glad.h>
#include <iostream>

namespace GL3 {

	Texture::Texture()
		: _target(GL_TEXTURE_2D), _textureID(0)
	{
		//! Do nothing
	}

	Texture::~Texture()
	{
		CleanUp();
	}

	void Texture::Initialize(GLenum target)
	{
		_target = target;

		glGenTextures(1, &_textureID);
		glBindTexture(_target, _textureID);
		glTexParameteri(_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void Texture::UploadTexture(void* data, int width, int height, GLenum format, GLenum internalFormat, GLenum type)
	{
		glTexImage2D(_target, 0, internalFormat, width, height, 0, format, type, data);
		glGenerateMipmap(_target);
	}

	void Texture::BindTexture(GLuint slot) const
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(_target, _textureID);
	}

	void Texture::UnbindTexture()
	{
		glBindTexture(_target, 0);
	}

	void Texture::CleanUp()
	{
		if (_textureID) glDeleteTextures(1, &_textureID);
	}

};