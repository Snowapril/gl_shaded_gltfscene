#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
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
		~Renderer();
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
		std::shared_ptr< GL3::Window > GetCurrentWindow() const;
	protected:
		virtual bool OnInitialize(const cxxopts::ParseResult& configure);
		virtual void OnCleanUp();
		virtual void OnUpdateFrame(double dt);
		virtual void OnDrawFrame();
		virtual void OnProcessInput(unsigned int key);

		std::weak_ptr< GL3::Application > _currentApp;

		std::vector< std::shared_ptr< GL3::Application > > _applications;

	private:
		void ProcessInput(unsigned int key);
	};
};

#endif //! end of Renderer.hpp