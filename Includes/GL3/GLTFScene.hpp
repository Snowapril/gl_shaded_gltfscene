#ifndef GLTF_SCENE_HPP
#define GLTF_SCENE_HPP

#include <string>

namespace tinygltf {
	class Model;
};

namespace GL3 {

	class GLTFScene
	{
	public:
		//! Default Constructor
		GLTFScene();
		//! Default Destructor
		~GLTFScene();

	protected:
	private:
		static bool LoadModel(tinygltf::Model* model, const std::string& filename);

	};

}

#endif