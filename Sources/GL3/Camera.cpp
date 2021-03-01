#include <GL3/Camera.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GL3 {

	Camera::Camera()
		: _projection(1.0f), _view(1.0f), _position(0.0f), 
		  _direction(0.0f, -1.0f, 0.0f), _up(0.0f, 1.0f, 0.0f), 
		  _lastCursorPos(0.0, 0.0), _speed(0.03f), _uniformBuffer(0)
	{
		//! Do nothing
	}

	Camera::~Camera()
	{
		CleanUp();
	}
	
	bool Camera::SetupUniformBuffer()
	{

		glGenBuffers(1, &_uniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 3, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		return true;
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
	
	void Camera::BindCamera() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
	}

	void Camera::UnbindCamera()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	
	GLuint Camera::GetUniformBuffer() const
	{
		return _uniformBuffer;
	}

	void Camera::UpdateMatrix()
	{
		this->_view = glm::lookAt(this->_position, this->_position + this->_direction, this->_up);

		OnUpdateMatrix();

		if (_uniformBuffer)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(_projection));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_view));
			glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, sizeof(glm::mat4), glm::value_ptr(_projection * _view));
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	void Camera::ProcessInput(unsigned int key)
	{
		switch (key)
		{
		case GLFW_KEY_W :
			this->_position += this->_direction * _speed;
			break;
		case GLFW_KEY_A:
			this->_position += glm::cross(this->_direction, this->_up) * _speed;
			break;
		case GLFW_KEY_S:
			this->_position -= this->_direction * _speed;
			break;
		case GLFW_KEY_D:
			this->_position -= glm::cross(this->_direction, this->_up) * _speed;
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

	void Camera::CleanUp()
	{
		if (_uniformBuffer) glDeleteBuffers(1, &_uniformBuffer);
	}
};