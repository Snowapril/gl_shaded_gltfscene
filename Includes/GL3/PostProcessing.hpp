#ifndef POST_PROCESSING_HPP
#define POST_PROCESSING_HPP

#include <GL3/GLTypes.hpp>
#include <GL3/Shader.hpp>
#include <glm/vec2.hpp>
#include <memory>

namespace GL3 {

	class PostProcessing
	{
	public:
		//! Default constructor
		PostProcessing();
		//! Default destructor
		~PostProcessing();
		//! Initialize the resources 
		bool Initialize();
		//! Rendering the post-processed screen image
		void Render() const;
		//! Resize the generated resoures
		void Resize(const glm::ivec2& extent);
		//! Cleanup the generated resources
		void CleanUp();
		//! Returns the framebuffer ID
		inline GLuint GetFramebuffer() const
		{
			return _fbo;
		}
	protected:
	private:
		GLuint _fbo;
		GLuint _color, _depth;
		GLuint _vao;
		std::unique_ptr< GL3::Shader > _shader;
	};

};

#endif //! end of PostProcessing.hpp