#include <iostream>
#include <cxxopts/cxxopts.hpp>

#include <GL3/Renderer.hpp>
#include <GL3/Window.hpp>
#include <chrono>

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

	GL3::Renderer renderer;
	if (!renderer.Initialize(result))
	{
		std::cerr << "Failed to initialize the Renderer" << std::endl;
		return EXIT_FAILURE;
	}
	
	std::shared_ptr< GL3::Window > window;
	auto startTime = std::chrono::steady_clock::now();
	while ((window = renderer.GetCurrentWindow()) && window->CheckWindowShouldClose() == false)
	{
		auto nowTime = std::chrono::steady_clock::now();
		double dt = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - startTime).count() / 1e-6;
		startTime = nowTime;

		renderer.UpdateFrame(dt);
		renderer.DrawFrame();

		window->SwapBuffer();
		window->PollEvents();
	}

	window.reset();
	return 0;
}