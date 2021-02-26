#include <iostream>
#include <cxxopts/cxxopts.hpp>

#include <GL3/Window.hpp>
#include <GL3/Camera.hpp>
#include <GL3/Mesh.hpp>
#include <GL3/Shader.hpp>

void Update();
void DrawFrame();
void ProcessInput();

int main(int argc, char* argv[])
{
	cxxopts::Options options("modern-opengl-template", "simple description");

	options.add_options()
		("d,debug", "Enable debugging") // a bool parameter
		("i,integer", "Int param", cxxopts::value<int>())
		("f,file", "File name", cxxopts::value<std::string>())
		("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"));

	auto result = options.parse(argc, argv);

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}

	GL3::Window window("modern-opengl-template", 1200, 900);

	GL3::Camera camera;
	camera.SetupCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	GL3::Shader shader;

	while (window.CheckWindowShouldClose() == false)
	{
		ProcessInput();
		Update();
		DrawFrame();

		window.SwapBuffer();
		window.PollEvents();
	}

	window.CleanUp();

	return 0;
}

void Update()
{

}
void DrawFrame()
{

}
void ProcessInput()
{

}