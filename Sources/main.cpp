#include <iostream>
#include <cxxopts/cxxopts.hpp>

#include <SampleRenderer.hpp>
#include <GL3/Window.hpp>
#include <glfw/glfw3.h>
#include <chrono>

int main(int argc, char* argv[])
{
	cxxopts::Options options("modern-opengl-template", "simple description");

	options.add_options()
		("t,title", "Window Title(default is 'modern-opengl-template')", cxxopts::value<std::string>()->default_value("modern-opengl-template"))
		("w,width", "Window width(default is 1200)", cxxopts::value<int>()->default_value("1200"))
		("h,height", "Window height(default is 900)", cxxopts::value<int>()->default_value("900"));

	auto result = options.parse(argc, argv);

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}

	auto renderer = std::make_unique<SampleRenderer>();
	if (!renderer->Initialize(result))
	{
		std::cerr << "Failed to initialize the Renderer" << std::endl;
		return EXIT_FAILURE;
	}
	
	std::shared_ptr< GL3::Window > window = renderer->GetWindow();
	auto startTime = std::chrono::steady_clock::now();
	while (!renderer->GetRendererShouldExit())
	{
		auto nowTime = std::chrono::steady_clock::now();
		double dt = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - startTime).count() / 1e-6;
		startTime = nowTime;

		renderer->UpdateFrame(dt);
		renderer->DrawFrame();
		
		glfwSwapBuffers(window->GetGLFWWindow());
		glfwPollEvents();
	}

	renderer->CleanUp();

	return 0;
}