#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <glm/vec2.hpp>
#include <string>
#include <vector>
#include <functional>

struct GLFWwindow;

namespace GL3
{
	class Window
	{
	public:
		//! Callback functions
		using KeyCallback = std::function<void(unsigned int)>;
		using CursorPosCallback = std::function<void(double, double)>;

		//! Constructor with title and extent
		Window(const std::string& title, int width, int height);
		//! Default destructor
		~Window();
		//! Initialize the window with given arguments
		bool Initialize(const std::string& title, int width, int height);
		//! Destroy the created window context
		void CleanUp();
		//! Returns the GLFWwindow pointer
		const GLFWwindow* GetWindow() const;
		//! Returns whether if the opened window must be closed
		bool CheckWindowShouldClose() const;
		//! Poll window events
		void PollEvents() const;
		//! Swap buffers
		void SwapBuffer() const;
		//! Process Input
		void ProcessInput() const;
		//! Mouse cursor position callback method.
		void ProcessCursorPos(double xpos, double ypos) const;
	protected:
		GLFWwindow* _window;
		std::string _windowTitle;
		glm::ivec2 _windowExtent;
	private:
		std::vector< KeyCallback > _keyCallbacks;
		std::vector< CursorPosCallback > _cursorPosCallbacks;
	};
};

#endif //! end of Window.hpp