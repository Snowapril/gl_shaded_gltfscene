#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL3/GLTypes.hpp>
#include <string>

namespace GL3 {

	class Texture
	{
	public:
		//! Default constructor
		Texture();
		//! Default destructor
		~Texture();
		//! Initialize the texture with given image path.
		void Initialize(GLenum target);
		//! Upload the texel data to the GPU memory
		void UploadTexture(void* data, int width, int height, GLenum format, GLenum internalFormat, GLenum type);
		//! Bind generated texture.
		void BindTexture(GLuint slot) const;
		//! Unbind texture with current bound slot
		//! declared as static because nothing related with member variables or method
		static void UnbindTexture();
		//! Clean up the generated resources
		void CleanUp();
	private:
		GLenum _target;
		GLuint _textureID;
	};

};

#endif //! end of Texture.hpp