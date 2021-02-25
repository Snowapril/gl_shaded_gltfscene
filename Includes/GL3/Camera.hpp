#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <GL3/GLTypes.hpp>

namespace GL3 {

	class Camera
	{
	public:
		//! Default constructor
		Camera();
		//! Default destructor
		virtual ~Camera();
		//! Setup camera position, direction and up vector.
		void SetupCamera(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up);
		//! Returns view matrix
		glm::mat4 GetViewMatrix();
		//! Returns projection matrix
		glm::mat4 GetProjectionMatrix();
		//! Update the matrix with specific methods, such as perspective or orthogonal.
		void UpdateMatrix();
		//! Process the continuous key input
		void ProcessInput(unsigned int key, double dt);
		//! Process the continuous mouse cursor position input
		void ProcessCursorPos(double xpos, double ypos);
	protected:
		virtual void OnUpdateMatrix() {};
		glm::mat4 _projection, _view;
		glm::vec3 _position, _direction, _up;
	private:
		glm::dvec2 _lastCursorPos;
		float _speed;
	};

};

#endif //! end of Camera.hpp