#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <GL3/GLTypes.hpp>
#include <cxxopts/cxxopts.hpp>

namespace GL3
{
	class Application;
	class Window;

	class Renderer
	{
	public:
		//! Default constructor
		Renderer();
		//! Default desctrutor
		virtual ~Renderer();
		//! Initialize the Renderer
		bool Initialize(const cxxopts::ParseResult& configure);
		//! Add application implementation
		bool AddApplication(std::shared_ptr<Application> app, const cxxopts::ParseResult& configure);
		//! Update the application with delta time.
		void UpdateFrame(double dt);
		//! Draw the one frame of the application.
		void DrawFrame();
		//! Clean up the all resources.
		void CleanUp();
		//! Returns the current bound application
		std::shared_ptr< GL3::Application > GetCurrentApplication() const;
		//! Returns the current application's window
		std::shared_ptr< GL3::Window > GetWindow() const;
		//! Returns whether this renderer should exit or not.
		bool GetRendererShouldExit() const;
		//! Switch the current app to the next given application
		void SwitchApplication(std::shared_ptr< GL3::Application > app);
		void SwitchApplication(size_t index);
	protected:
		virtual bool OnInitialize(const cxxopts::ParseResult& configure) = 0;
		virtual void OnCleanUp() = 0;
		virtual void OnUpdateFrame(double dt) = 0;
		virtual void OnBeginDraw() = 0;
		virtual void OnEndDraw() = 0;
		virtual void OnProcessInput(unsigned int key) = 0;

		//! Begin of GPU Time measurement
		void BeginGPUMeasure();
		//! End of GPU Time measurement and returns elapsed time
		size_t EndGPUMeasure();

		std::weak_ptr< GL3::Application > _currentApp;
		std::vector< std::shared_ptr< GL3::Application > > _applications;
		std::shared_ptr< GL3::Window > _mainWindow;
		std::vector< std::shared_ptr< GL3::Window > > _sharedWindows;
	private:
		//! Process the input key
		void ProcessInput(unsigned int key);
		//!Process the mouse cursor positions
		void ProcessCursorPos(double xpos, double ypos);

		GLuint _queryID;
		bool _bMeasureGPUTime;
	};
};

#endif //! end of Renderer.hpp