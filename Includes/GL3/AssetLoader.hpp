#ifndef ASSET_LOADER_HPP
#define ASSET_LOADER_HPP

#include <GL3/GLTypes.hpp>
#include <GL3/Vertex.hpp>
#include <string>
#include <vector>

namespace GL3 {

	class AssetLoader
	{
	public:
		//! load obj file and returns vertices and indices to given reference argument.
		//! Vertices are automatically formatted with given format
		static bool LoadObjFile(const std::string& path, std::vector<float>& vertices, std::vector<unsigned int>& indices, VertexFormat format);
		//! Load raw binary file and returns them to given reference argument.
		static bool LoadRawFile(const std::string& path, std::vector<char>& data);
	};

};

#endif //! end of AssetLoader.hpp