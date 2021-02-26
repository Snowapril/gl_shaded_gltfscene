#include <GL3/PerspectiveCamera.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GL3 {

	PerspectiveCamera::PerspectiveCamera()
		: _aspect(1.0f), _fovDegree(60.0f), _zNear(0.1f), _zFar(100.0f)
	{
		//! Do nothing
	}

	PerspectiveCamera::~PerspectiveCamera()
	{
		//! Do nothing
	}

	void PerspectiveCamera::SetProperties(float aspect, float fovDegree, float zNear, float zFar)
	{
		this->_aspect	 = aspect;
		this->_fovDegree = fovDegree;
		this->_zNear	 = zNear;
		this->_zFar		 = zFar;
	}

	void PerspectiveCamera::OnUpdateMatrix()
	{
		this->_projection = glm::perspective(glm::radians(this->_fovDegree), this->_aspect, this->_zNear, this->_zFar);
	}
};