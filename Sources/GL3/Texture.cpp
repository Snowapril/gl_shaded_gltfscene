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
		glCreateTextures(target, 1, &_textureID);
		glTextureParameteri(_textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void Texture::UploadTexture(const void* data, int width, int height, GLenum internalFormat, GLenum format, GLenum type)
	{
		glTextureStorage2D(_textureID, 1, internalFormat, width, height);
		if (data)
		{
			glTextureSubImage2D(_textureID, 0, 0, 0, width, height, format, type, data);
			glGenerateTextureMipmap(_textureID);
		}
	}

	void Texture::UploadTextureArray(const void* data, int width, int height, int layerCount, GLenum internalFormat, GLenum format, GLenum type)
	{
		glTextureStorage3D(_textureID, 1, internalFormat, width, height, layerCount);
		if (data)
		{
			glTextureSubImage3D(_textureID, 1, 0, 0, 0, width, height, layerCount, format, type, data);
			glGenerateTextureMipmap(_textureID);
		}
	}

	void Texture::BindTexture(GLuint slot) const
	{
		glBindTextureUnit(slot, _textureID);
	}

	void Texture::UnbindTexture() const
	{
		glBindTexture(_target, 0);
	}

	void Texture::CleanUp()
	{
		if (_textureID) glDeleteTextures(1, &_textureID);
	}

	GLuint Texture::GetResourceID() const
	{
		return _textureID;
	}
};