#include <iostream>
#include <cxxopts/cxxopts.hpp>


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

	return 0;
}