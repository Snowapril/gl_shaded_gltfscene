#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glm/vec2.hpp>
#include <string>
#include <vector>
#include <functional>

struct GLFWwindow;

namespace GL3
{
	//!
	//! \brief      Window context and callback managing class
	//!
	//! This class manage window resources(cross-platform) and handling multiple callback methods.
	//! The other classes, by adding callback method to this window class, enable to handle inputs.
	//!
	class Window
	{
	public:
		//! Callback functions
		using KeyCallback = std::function<void(unsigned int)>;
		using CursorPosCallback = std::function<void(double, double)>;
		using ResizeCallback = std::function<void(int, int)>;
		//! Default constructor
		Window();
		//! Constructor with title and extent
		Window(const std::string& title, int width, int height);
		//! Default destructor
		~Window();
		//!
		//! \brief      Create the GLFWwindow with given arguments
		//!
		//! \param title - window title
		//! \param width - window screen width
		//! \param height - window screen height
		//! \param sharedWindow - if sharedWindow is not nullptr(e.g. generated window in advance), create shared context
		//!
		bool Initialize(const std::string& title, int width, int height, GLFWwindow* sharedWindow = nullptr);
		//! Destroy the created window context
		void CleanUp();
		//! Returns the GLFWwindow pointer
		GLFWwindow* GetGLFWWindow();
		//! Returns the window extent
		glm::ivec2 GetWindowExtent() const;
		//! Process Input
		void ProcessInput() const;
		//! Mouse cursor position callback method.
		void ProcessCursorPos(double xpos, double ypos) const;
		//! Screen resize callback method.
		void ProcessResize(int width, int height);
		//! Add input callback functions
		void operator+=(const KeyCallback& callback);
		//! Add cursor position callback functions
		void operator+=(const CursorPosCallback& callback);
		//! Add screen resize callback functions
		void operator+=(const ResizeCallback& callback);
		//! Returns the window extent aspect ratio.
		float GetAspectRatio() const;
	protected:
		GLFWwindow* _window;
		std::string _windowTitle;
		glm::ivec2 _windowExtent;
	private:
		std::vector< KeyCallback > _keyCallbacks;
		std::vector< CursorPosCallback > _cursorPosCallbacks;
		std::vector< ResizeCallback > _resizeCallbacks;
	};
};

#endif //! end of Window.hpp