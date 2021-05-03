#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL3/GLTypes.hpp>
#include <Core/GLTFScene.hpp>
#include <Core/Vertex.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

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
		//! Render the whole nodes of the parsed gltf-scene
		void Render(const std::shared_ptr< Shader >& shader, GLenum alphaMode);
		//! Clean up the generated resources
		void CleanUp();
	private:
		std::unordered_map< std::string, std::shared_ptr< Texture > > _textures;
		std::vector< GLuint > _buffers;
		GLuint _vao, _ebo;
	};

};

#endif //! end of Scene.hpp