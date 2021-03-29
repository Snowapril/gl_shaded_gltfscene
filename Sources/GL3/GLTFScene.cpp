#include <GL3/GLTFScene.hpp>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION //! optional. disable exception handling.
#define TINYGLTF_ENABLE_DRACO
#if defined(_MSC_VER)
	#pragma warning(disable:4100)
	#pragma warning(disable:4804)
	#pragma warning(disable:4127)
	#pragma warning(disable:4018)
#endif
#include <tinygltf/tiny_gltf.h>

namespace GL3 {

	GLTFScene::GLTFScene()
	{
		//! Do nothing
	}

	GLTFScene::~GLTFScene()
	{
		//! Do nothing
	}
	
	bool GLTFScene::Initialize(const std::string& filename, VertexFormat format)
	{
		tinygltf::Model model;
		if (!LoadModel(&model, filename))
			return false;
		
		
		return true;
	}

	bool GLTFScene::LoadModel(tinygltf::Model* model, const std::string& filename)
	{
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		bool res = loader.LoadBinaryFromFile(model, &err, &warn, filename);
		if (!res) loader.LoadASCIIFromFile(model, &err, &warn, filename);

		if (!res) std::cerr << "Failed to load GLTF model : " << filename << std::endl;

		return res;
	}

};
