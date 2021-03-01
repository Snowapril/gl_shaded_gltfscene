#include <GL3/Shader.hpp>
#include <GL3/DebugUtils.hpp>
#include <glad/glad.h>
#include <fstream>
#include <iostream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

std::string GetPathDirectory(const std::string& path)
{
	size_t pos = path.find_last_of('/');
	return path.substr(0, pos + 1);
}

std::string PreprocessShaderInclude(const std::string& path, std::string includePrefix = "#include")
{
	includePrefix += ' ';

	//! Load shader source file.
	std::ifstream file(path);

	if (file.is_open() == false)
	{
		std::cerr << "Open shader source file failed" << path << std::endl;
		GL3::StackTrace::PrintStack();
	}

	std::string fullSourceCode = "";
	std::string temp;
	while (std::getline(file, temp))
	{
		if (temp.find(includePrefix) != std::string::npos) //! temp string contain "#include "
		{
			temp.erase(0, includePrefix.length());

			std::string filePath = GetPathDirectory(path);
			filePath += temp;
			std::string includeSrc = PreprocessShaderInclude(filePath);

			fullSourceCode += includeSrc;
			continue;
		}

		fullSourceCode += temp + '\n';
	}

	file.close();
	return fullSourceCode;
}

namespace GL3 {

	Shader::Shader()
		: _programID(0)
	{
		//! Do nothing
	}

	Shader::~Shader()
	{
		CleanUp();
	}

	bool Shader::Initialize(const std::unordered_map<GLenum, std::string>& sources)
	{
		std::vector<GLuint> compiledShaders;
		for (const auto& sourcePair : sources)
		{
			const GLenum type = sourcePair.first;
			const std::string& path = sourcePair.second;

			//! Load shader file contents handled with #include.
			const std::string contents = PreprocessShaderInclude(path);
			const char* source = contents.c_str();

			GLuint shader = glCreateShader(type);
			glShaderSource(shader, 1, &source, nullptr);
			glCompileShader(shader);
			
			int success;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				int length;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
				std::vector<GLchar> logs(length);
				glGetShaderInfoLog(shader, length, nullptr, logs.data());
				std::clog << "Shader Compile Error Log" << std::endl;
				std::clog << logs.data() << std::endl;
				StackTrace::PrintStack();
				return false;
			}

			compiledShaders.push_back(shader);
		}

		_programID = glCreateProgram();
		for (GLuint shader : compiledShaders)
		{
			glAttachShader(_programID, shader);
			glDeleteShader(shader);
		}
		glLinkProgram(_programID);

		int success;
		glGetProgramiv(_programID, GL_LINK_STATUS, &success);
		if (!success)
		{
			int length;
			glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &length);
			std::vector<GLchar> logs(length);
			glGetProgramInfoLog(_programID, length, nullptr, logs.data());
			std::clog << "Program Linking Error Log" << std::endl;
			std::clog << logs.data() << std::endl;
			StackTrace::PrintStack();
			return false;
		}

		return true;
	}

	void Shader::BindShaderProgram() const
	{
		glUseProgram(this->_programID);
	}
	
	void Shader::UnbindShaderProgram()
	{
		glUseProgram(0);
	}

	void Shader::BindUniformBlock(const std::string& blockName, GLuint bindingPoint) const
	{
		glUniformBlockBinding(_programID, glGetUniformBlockIndex(_programID, blockName.c_str()), bindingPoint);
	}

	bool Shader::HasUniformVariable(const std::string& name)
	{
		if (_uniformCache.count(name) != 0)
			return true;

		GLint loc = GetUniformLocation(name);

		return loc != -1;
	}

	GLint Shader::GetUniformLocation(const std::string& name)
	{
		auto iter = _uniformCache.find(name);

		if (iter == _uniformCache.end())
		{
			GLint loc = glGetUniformLocation(this->_programID, name.c_str());
			
			if (loc == -1)
				return -1;

			_uniformCache.emplace(name, loc);
			return loc;
		}
		else
		{
			return iter->second;
		}
	}

	void Shader::CleanUp()
	{
		if (_programID)
			glDeleteProgram(_programID);
	}

	template <>
	void Shader::SendUniformVariable(const std::string& name, int&& val)
	{
		GLint loc = GetUniformLocation(name);
		glUniform1i(loc, val);
	}

	template <>
	void Shader::SendUniformVariable(const std::string& name, float&& val)
	{
		GLint loc = GetUniformLocation(name);
		glUniform1f(loc, val);
	}

	template <>
	void Shader::SendUniformVariable(const std::string& name, glm::vec3&& val)
	{
		GLint loc = GetUniformLocation(name);
		glUniform3fv(loc, 1, glm::value_ptr(val));
	}

	template <>
	void Shader::SendUniformVariable(const std::string& name, glm::vec4&& val)
	{
		GLint loc = GetUniformLocation(name);
		glUniform4fv(loc, 1, glm::value_ptr(val));
	}

	template <>
	void Shader::SendUniformVariable(const std::string& name, glm::mat4&& val)
	{
		GLint loc = GetUniformLocation(name);
		glm::mat4 movedVal = std::move(val);
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(movedVal));
	}

};