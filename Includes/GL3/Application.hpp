#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <cxxopts/cxxopts.hpp>

namespace GL3
{
	class Window;
	class Camera;
	class Shader;
	class Texture;

	class Application
	{
	public:
		//! Default constructor
		Application();
		//! Default desctrutor
		~Application();
		//! Initialize the Application
		bool Initialize(const cxxopts::ParseResult& configure);
		//! Window instance to the application
		bool InitWindow(const std::string& title, int width, int height);
		//! Add camera instance with Perspective or Orthogonal
		void AddCamera(std::shared_ptr< GL3::Camera >&& camera);
		//! Update the application with delta time.
		void Update(double dt);
		//! Draw the one frame of the application.
		void Draw();
		//! Clean up the all resources.
		void CleanUp();
		//! Returns the current bound window
		std::shared_ptr< GL3::Window > GetWindow() const;
	protected:
		virtual bool OnInitialize(const cxxopts::ParseResult& configure) = 0;
		virtual void OnCleanUp() = 0;
		virtual void OnUpdate(double dt) = 0;
		virtual void OnDraw() = 0;

		std::shared_ptr< GL3::Window > _window;

		std::vector< std::shared_ptr< GL3::Camera > > _cameras;
		std::unordered_map< std::string, std::shared_ptr< GL3::Shader > > _shaders;
		std::unordered_map< std::string, std::shared_ptr< GL3::Texture > > _textures;

	private:
		void ProcessInput(unsigned int key);
		void ProcessCursorPos(double xpos, double ypos);
	};
};

#endif //! end of Application.hpp