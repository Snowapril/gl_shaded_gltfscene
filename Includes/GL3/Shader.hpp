#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL3/GLTypes.hpp>
#include <unordered_map>
#include <string>

namespace GL3 {

	class Shader
	{
	public:
		//! Default constructor
		Shader();
		//! Default destructor
		~Shader();
		//! Initialize the shader program with given shader source files.
		//! \param sources : a pair of shader type and it's source code file path.
		bool Initialize(const std::unordered_map<GLenum, std::string>& sources);
		//! Bind generated shader program.
		void BindShaderProgram() const;
		//! Unbind shader program
		//! declared as static because nothing related with member variables or method
		static void UnbindShaderProgram();
		//! Bind uniform block to this program
		void BindUniformBlock(const std::string& blockName, GLuint bindingPoint) const;
		//! Check this shader program has uniform variable with given name
		bool HasUniformVariable(const std::string& name);
		//! Returns the uniform variable location matched with given name.
		GLint GetUniformLocation(const std::string& name);
		//! Send the uniform variable to the pipeline.
		template <typename Type>
		void SendUniformVariable(const std::string& name, Type val);
		//! Clean up the generated resources
		void CleanUp();
	private:
		std::unordered_map<std::string, GLint> _uniformCache;
		GLuint _programID;
	};

};

#endif //! end of Shader.hpp