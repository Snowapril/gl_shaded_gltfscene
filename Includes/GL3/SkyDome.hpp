#ifndef SKYDOME_HPP
#define SKYDOME_HPP

#include <GL3/DebugUtils.hpp>
#include <GL3/GLTypes.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <glm/vec2.hpp>

namespace GL3 {

	class Texture;
	class Shader;

	//!
	//! \brief      Skydome environment map for Image Based Lighting
	//! 
	//! This skydome require hdr environment map image input and generates multiple textures.
	//! [hdrTexture] : texture 2d resource contain given hdr envionment image
	//! [accelTexture] : acceleration texture for speed-up generate several filters and brdf LUT
	//! [brdflUT] : brdf lookup table texture which can be precalculated
	//! [irradianceCube] : prefiltered diffuse texture which can be precalculated
	//! [prefilteredCube] : prefiltered glossy texture which can be precalculated
	//!
	class SkyDome
	{
	public:
		//! Default constructor
		SkyDome();
		//! Default destructor
		~SkyDome();
		//! Initialize Skydome with hdr environment map filepath.
		//! This method will load hdr image and create each corresponded textures
		bool Initialize(const std::string& envPath);
		//! Render skydoem environment to screen
		void Render(const std::shared_ptr< Shader >& shader, GLenum alphaMode);
		//! Clean up the generated resources
		void CleanUp();

		struct IBLTextureSet {
			GLuint brdfLUT{ 0 };
			GLuint irradianceCube{ 0 };
			GLuint prefilteredCube{ 0 };
			GLuint hdrTexture{ 0 };
			GLuint accelTexture{ 0 };
		};

		//! Returns const reference of IBL texture set.
		const IBLTextureSet& GetIBLTextureSet() const;
	private:
		void CreateCube();
		void RenderToCube(GLuint fbo, GLuint texture, Shader* shader, unsigned int dim, const unsigned int numMips);
		void CreateEnvironmentAccelTexture(const float* pixels, glm::vec2 size, GLuint accelTexture);
		void IntegrateBRDF(unsigned int dim);
		void PrefilterDiffuse(unsigned int dim);
		void PrefilterGlossy(unsigned int dim);

		IBLTextureSet _textureSet;
		GLuint _vao{ 0 }, _vbo{ 0 }, _ebo{ 0 };
		DebugUtils _debug;
	};

};

#endif //! end of SkyDome.hpp