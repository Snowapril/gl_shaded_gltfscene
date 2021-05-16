#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL3/GLTypes.hpp>
#include <unordered_map>
#include <string>

namespace GL3 {

	//!
	//! \brief      Program with attached arbitrary shaders
	//!
	//! Enable to have multiple applciation and multiple context with one main shared context.
	//! This class provides render & update routine and profiling GPU time features.
	//! Pass whole input callbacks such as mouse, keyboard into GLFWwindow callback function collection.
	//! 
	class Shader
	{
	public:
		//! Default constructor
		Shader();
		//! Default destructor
		~Shader();
		//!
		//! \brief Compile given shader files link the program
		//!
		//! \param sources - pairs of shader type and shader file path collection.
		//!
		bool Initialize(const std::unordered_map<GLenum, std::string>& sources);
		//! Bind generated shader program.
		void BindShaderProgram() const;
		//! Unbind shader program
		//! declared as static because nothing related with member variables or method
		static void UnbindShaderProgram();
		//! Bind uniform block to this program
		void BindUniformBlock(const std::string& blockName, GLuint bindingPoint) const;
		//! Bind frag data location (STANDARD)
		void BindFragDataLocation(const std::string& name, GLuint location) const;
		//! Check this shader program has uniform variable with given name
		bool HasUniformVariable(const std::string& name);
		//! Returns the uniform variable location matched with given name.
		GLint GetUniformLocation(const std::string& name);
		//! Send the uniform variable to the pipeline.
		template <typename Type>
		void SendUniformVariable(const std::string& name, Type val);
		//! Clean up the generated resources
		void CleanUp();
		//! Returns the program resource ID
		GLuint GetResourceID() const;
	private:
		std::unordered_map<std::string, GLint> _uniformCache;
		GLuint _programID;
	};

};

#endif //! end of Shader.hpp