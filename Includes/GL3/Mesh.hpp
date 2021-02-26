#ifndef MESH_HPP
#define MESH_HPP

#include <glm/vec3.hpp>
#include <GL3/GLTypes.hpp>
#include <GL3/BoundingBox.hpp>

namespace GL3 {

	class Mesh
	{
	public:
		//! Default constructor
		Mesh();
		//! Default destructor
		~Mesh();
		//! Load vertices data from the obj file.
		bool LoadObj(const char* path, bool scaleToUnitBox = true);
		//! Draw the loaded and generated mesh with given primitive mode
		void DrawMesh(GLenum mode);
		//! Clean up the generated resources
		void CleanUp();
	private:
		BoundingBox _boundingBox;
		GLuint _vao, _vbo, _ebo;
		unsigned int _numVertices;
	};

}; 

#endif //! end of Mesh.hpp