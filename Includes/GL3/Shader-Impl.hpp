#ifndef SHADER_IMPL_HPP
#define SHADER_IMPL_HPP

namespace GL3 {

	template <typename Type>
	void Shader::SendUniformVariable(const std::string& name, Type&& val)
	{
		static_assert("No implementation exists");
	}
};

#endif //! end of Shader-Impl.hpp