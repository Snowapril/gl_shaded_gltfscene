#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL3/GLTypes.hpp>
#include <Core/GLTFScene.hpp>
#include <Core/Vertex.hpp>
#include <glm/mat4x4.hpp>
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
		//! Scene node matrix type definition with pair of glm::mat4.
		using NodeMatrix = std::pair<glm::mat4, glm::mat4>;
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
		std::vector< std::shared_ptr< Texture > > _textures;
		std::vector< GLuint > _buffers;
		GLuint _vao, _ebo;
		GLuint _matrixUBO;
	};

};

#endif //! end of Scene.hpp