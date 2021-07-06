//! Interpolation methods referenced on 
//! https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md

#ifndef MATHUTILS_IMPL_HPP
#define MATHUTILS_IMPL_HPP

#include <Core/Macros.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Core
{
	namespace Interpolation
	{
		template <typename Type>
		Type Lerp(Type prev, Type next, const float keyframe)
		{
			return (1.0f - keyframe) * prev + keyframe * next;
		}

		template <typename Type>
		Type SLerp(Type prev, Type next, const float keyframe)
		{
			float dotProduct = glm::dot(prev, next);

			//! Make sure we take the shortest path in case dot product is negative
			if (dotProduct < 0.0)
			{
				next = -next;
				dotProduct = -dotProduct;
			}

			//! If the two quaternions are too close to each other, just linear interpolate between the 4D vector
			if (dotProduct > 0.9995)
				return glm::normalize((1.0f - keyframe) * prev + keyframe * next);

			//! Perform the spherical linear interpolation
			float theta0 = std::acos(dotProduct);
			float theta = keyframe * theta0;
			float sinTheta = std::sin(theta);
			float sinTheta0Inv = 1.0 / (std::sin(theta0) + 1e-6);

			float scalePrevQuat = std::cos(theta) - dotProduct * sinTheta * sinTheta0Inv;
			float scaleNextQuat = sinTheta * sinTheta0Inv;
			return scalePrevQuat * prev + scaleNextQuat * next;
		}

		template <typename Type>
		Type CubicSpline(Type prev, Type next, const float keyframe)
		{
			UNUSED_VARIABLE(prev);
			UNUSED_VARIABLE(next);
			UNUSED_VARIABLE(keyframe);
			static_assert("Not implemented yet");
		}

		template <typename Type>
		Type Step(Type prev, Type next, const float keyframe)
		{
			return keyframe >= 1.0 ? next : prev;
		}
	};
};

#endif //! end of MathUtils-Impl.hpp