#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <functional>

namespace Core
{
	template <typename Type>
	using InterpolationMethod = std::function<Type(Type, Type, const float)>;

	namespace Interpolation
	{
		template <typename Type>
		Type Lerp(Type prev, Type next, const float keyframe);

		template <typename Type>
		Type SLerp(Type prev, Type next, const float keyframe);

		template <typename Type>
		Type CubicSpline(Type prev, Type next, const float keyframe);

		template <typename Type>
		Type Step(Type prev, Type next, const float keyframe);
	};
};

#include <Core/MathUtils-Impl.hpp>
#endif //! end of MathUtils.hpp