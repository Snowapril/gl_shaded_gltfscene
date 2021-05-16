#ifndef ASSET_LOADER_HPP
#define ASSET_LOADER_HPP

#include <GL3/GLTypes.hpp>
#include <Core/Vertex.hpp>
#include <string>
#include <vector>

namespace Core {

	//!
	//! \brief      Collection of asset loader functions.
	//!
	class AssetLoader
	{
	public:
		//! load obj file and returns vertices and indices to given reference argument.
		//! Vertices are automatically formatted with given format
		static bool LoadObjFile(const std::string& path, std::vector<float>& vertices, std::vector<unsigned int>& indices, Core::VertexFormat format);
		//! Load raw binary file and returns them to given reference argument.
		static bool LoadRawFile(const std::string& path, std::vector<char>& data);
		//! Load float-per-channel image and returns the dynamic allocated pointer 
		//! and pass dimension to reference argument.
		//! It returns nullptr on failed
		[[nodiscard]] static float* LoadImageFile(const std::string& path, int* width, int* height, int* channel);
		//! Free the heap-allocated image data
		//! As this free require stb library, add one more static function
		static void FreeImage(void* pixels);
	};

};

#endif //! end of AssetLoader.hpp