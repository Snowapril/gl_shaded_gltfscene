#ifndef GLTF_SCENE_HPP
#define GLTF_SCENE_HPP

#include <GL3/Vertex.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <string>
#include <tinygltf/tiny_gltf.h>

namespace tinygltf {
	class Model;
};

namespace GL3 {

	//!
	//! \brief      GLTF scene file loader class
	//!
	//! After parsing given gltf file, this class provides meshes and material information
	//! about parsed model.
	//! 
	class GLTFScene
	{
	public:
		//! Default Constructor
		GLTFScene();
		//! Default Destructor
		~GLTFScene();
		//! Initialize the GLTFScene with gltf scene file path
		bool Initialize(const std::string& filename, VertexFormat format);

		struct GLTFNode
		{
			glm::mat4 world{ 1.0f };
			int primMesh{ 0 };
		};

		struct GLTFPrimMesh
		{

		};

		struct GLTFCamera
		{
			glm::mat4 world{ 1.0f };
			glm::vec3 eye{ 0.0f };
			glm::vec3 center{ 0.0f };
			glm::vec3 up{ 0.0f, 1.0f, 0.0f };

			tinygltf::Camera camera;
		};
	protected:
		std::vector<GLTFNode> _sceneNodes;
		std::vector<GLTFPrimMesh> _scenePrimMeshes;
		std::vector<GLTFCamera> _sceneCameras;
	private:
		static bool LoadModel(tinygltf::Model* model, const std::string& filename);

	};

}

#endif