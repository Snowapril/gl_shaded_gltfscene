#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL3/GLTypes.hpp>
#include <Core/GLTFScene.hpp>
#include <Core/Vertex.hpp>
#include <unordered_map>
#include <string>
#include <memory>

namespace GL3 {

	class Texture;
	class Shader;

	//!
	//! \brief      GLTF Scene rendering class
	//!
	class Scene : public Core::GLTFScene
	{
	public:
		//! Default constructor
		Scene();
		//! Default destructor
		~Scene();
		//! Load GLTFScene from the given scene filename and generate buffers 
		bool Initialize(const std::string& filename, Core::VertexFormat format);
		//! Clean up the generated resources
		void CleanUp();
	private:
		std::unordered_map< std::string, std::shared_ptr< Texture > > _textures;
	};

};

#endif //! end of Scene.hpp