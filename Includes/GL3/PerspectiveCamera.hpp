#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include <GL3/Camera.hpp>

namespace GL3 {

	class PerspectiveCamera : public Camera
	{
	public:
		//! Default constructor
		PerspectiveCamera();
		//! Default destructor
		~PerspectiveCamera();
		//! Set perspective camera properties
		void SetProperties(float aspect, float fovDegree, float zNear, float zFar);
	private:
		//! Update perpsective projection matrix;
		void OnUpdateMatrix() override;

		float _aspect;
		float _fovDegree;
		float _zNear;
		float _zFar;
	};

};

#endif //! end of PerspectiveCamera.hpp