#include <GL3/Camera.hpp>
#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>

namespace GL3 {

	Camera::Camera()
		: _projection(1.0f), _view(1.0f), _position(0.0f), 
		  _direction(0.0f, -1.0f, 0.0f), _up(0.0f, 1.0f, 0.0f), _speed(0.03f)
	{
		//! Do nothing
	}

	Camera::~Camera()
	{
		//! Do nothing
	}

	void Camera::SetupCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up)
	{
		this->_position = pos;
		this->_direction = dir;
		this->_up = up;
	}

	glm::mat4 Camera::GetViewMatrix()
	{
		return this->_view;
	}

	glm::mat4 Camera::GetProjectionMatrix()
	{
		return this->_projection;
	}
	
	void Camera::UpdateMatrix()
	{
		this->_view = glm::lookAt(this->_position, this->_position + this->_direction, this->_up);

		OnUpdateMatrix();
	}

	void Camera::ProcessInput(unsigned int key, double dt)
	{
		const float speed = _speed * static_cast<float>(dt);

		switch (key)
		{
		case GLFW_KEY_W :
			this->_position += this->_direction * speed;
			break;
		case GLFW_KEY_A:
			this->_position += glm::cross(this->_direction, this->_up) * speed;
			break;
		case GLFW_KEY_S:
			this->_position -= this->_direction * speed;
			break;
		case GLFW_KEY_D:
			this->_position -= glm::cross(this->_direction, this->_up) * speed;
			break;
		default:
			return;
		}

		UpdateMatrix();
	}

	void Camera::ProcessCursorPos(double xpos, double ypos)
	{
		static bool bFirstCall = true;
		const glm::dvec2 cursorPos(xpos, ypos);
		if (bFirstCall)
		{
			_lastCursorPos = cursorPos;
			bFirstCall = false;
		}

		constexpr float sensitivity = 8e-4f;
		const float xoffset = static_cast<float>(cursorPos.x - _lastCursorPos.x) * sensitivity;
		const float yoffset = static_cast<float>(_lastCursorPos.y - cursorPos.y) * sensitivity;
		
		//! create quaternion matrix with up vector and yaw angle.
		auto yawQuat	= glm::angleAxis(glm::radians(xoffset), this->_up);
		//! create quaternion matrix with right vector and pitch angle.
		auto pitchQuat	= glm::angleAxis(glm::radians(yoffset), glm::cross(this->_direction, this->_up));

		this->_direction = (yawQuat * pitchQuat * this->_direction);
		UpdateMatrix();
	}
};