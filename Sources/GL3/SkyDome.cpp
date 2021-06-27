#include <GL3/SkyDome.hpp>
#include <GL3/Shader.hpp>
#include <Core/Macros.hpp>
#include <Core/AssetLoader.hpp>
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>
#include <cmath>

namespace GL3 {

	SkyDome::SkyDome()
	{
		//! Do nothing
	}

	SkyDome::~SkyDome()
	{
		CleanUp();
	}

	bool SkyDome::Initialize(const std::string& envPath)
	{
		std::cout << "Loading Environment Map : " << envPath << '\n';
		int width, height, channels;
		float* pixels = Core::AssetLoader::LoadImageFile(envPath, &width, &height, &channels);

		if (pixels == nullptr)
			return false;
		else
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &_textureSet.hdrTexture);
			glTextureParameteri(_textureSet.hdrTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_textureSet.hdrTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_textureSet.hdrTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_textureSet.hdrTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureStorage2D(_textureSet.hdrTexture, 1, GL_RGBA32F, width, height);
			glTextureSubImage2D(_textureSet.hdrTexture, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);

			glCreateTextures(GL_TEXTURE_2D, 1, &_textureSet.accelTexture);
			glTextureParameteri(_textureSet.accelTexture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_textureSet.accelTexture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(_textureSet.accelTexture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(_textureSet.accelTexture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			CreateEnvironmentAccelTexture(pixels, glm::vec2(width, height), _textureSet.accelTexture);

			Core::AssetLoader::FreeImage(pixels);
		}

		if (_vao == 0)
			CreateCube();

		IntegrateBRDF(512);
		PrefilterDiffuse(128);
		PrefilterGlossy(512);

		_debug.SetObjectName(GL_TEXTURE, _textureSet.hdrTexture, "SkyHdr");
		_debug.SetObjectName(GL_TEXTURE, _textureSet.accelTexture, "SkyImpSamp");
		_debug.SetObjectName(GL_TEXTURE, _textureSet.brdfLUT, "SkyLut");
		_debug.SetObjectName(GL_TEXTURE, _textureSet.prefilteredCube, "SkyGlossy");
		_debug.SetObjectName(GL_TEXTURE, _textureSet.irradianceCube, "SkyIrradiance");

		return true;
	}

	void SkyDome::Render(const std::shared_ptr< Shader >& shader, GLenum alphaMode)
	{
		UNUSED_VARIABLE(shader);
		UNUSED_VARIABLE(alphaMode);

		auto renderScope = _debug.ScopeLabel("SkyDome Rendering");

		glDisable(GL_DEPTH_TEST);
		glBindTextureUnit(0, _textureSet.hdrTexture);
		glBindVertexArray(_vao);
		glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

	const SkyDome::IBLTextureSet& SkyDome::GetIBLTextureSet() const
	{
		return _textureSet;
	}

	void SkyDome::CleanUp()
	{
		glDeleteTextures(5, reinterpret_cast<GLuint*>(&_textureSet));
		if (_vbo) glDeleteBuffers(1, &_vbo);
		if (_ebo) glDeleteBuffers(1, &_ebo);
		if (_vao) glDeleteVertexArrays(1, &_vao);
	}

	void SkyDome::CreateCube()
	{
		//! Already cube is initialized	
		if (_vao != 0) return;

		std::vector<glm::vec3> vertexBuffer = {
			{-0.5f, -0.5f,	0.5f},  {0.5f, -0.5f,  0.5f},
			{-0.5f,  0.5f,	0.5f},  {0.5f,  0.5f,  0.5f},
			{-0.5f, -0.5f, -0.5f},	{0.5f, -0.5f, -0.5f},
			{-0.5f,  0.5f, -0.5f},  {0.5f,  0.5f, -0.5f},
		};

		std::vector<unsigned int> indexBuffer = {
			0, 3, 2, 0, 1, 3,  //    6-----7     Y
			1, 7, 3, 1, 5, 7,  //   /|    /|     ^
			5, 4, 7, 6, 7, 4,  //  2-----3 |     |
			0, 6, 4, 0, 2, 6,  //  | 4 --|-5     ---> X
			2, 7, 6, 2, 3, 7,  //  |/    |/     /
			4, 1, 0, 4, 5, 1,  //  0-----1     Z
		};

		glCreateVertexArrays(1, &_vao);
		glCreateBuffers(1, &_vbo);
		glNamedBufferStorage(_vbo, vertexBuffer.size() * sizeof(glm::vec3), vertexBuffer.data(), GL_MAP_READ_BIT);
		glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(glm::vec3));
		glEnableVertexArrayAttrib(_vao, 0);
		glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribBinding(_vao, 0, 0);

		glCreateBuffers(1, &_ebo);
		glNamedBufferStorage(_ebo, indexBuffer.size() * sizeof(unsigned int), indexBuffer.data(), GL_MAP_READ_BIT);
		glVertexArrayElementBuffer(_vao, _ebo);

		_debug.SetObjectName(GL_VERTEX_ARRAY, _vao, "SkyDome Cube Vertex Array");
		_debug.SetObjectName(GL_BUFFER, _vbo, "SkyDome Cube Vertex Buffer");
		_debug.SetObjectName(GL_BUFFER, _ebo, "SkyDome Cube Element Buffer");
	}

	void SkyDome::RenderToCube(GLuint fbo, GLuint texture, Shader* shader, unsigned int dim, const unsigned int numMips)
	{
		if (_vao == 0) CreateCube();

		glm::mat4 mv[6];
		const glm::vec3 pos(0.0f, 0.0f, 0.0f);
		mv[0] = glm::lookAt(pos, glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec3(0.0f, -1.0f, 0.0f));   // Positive X
		mv[1] = glm::lookAt(pos, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));  // Negative X
		mv[2] = glm::lookAt(pos, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));  // Positive Y
		mv[3] = glm::lookAt(pos, glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f));    // Negative Y
		mv[4] = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec3(0.0f, -1.0f, 0.0f));   // Positive Z
		mv[5] = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));  // Negative Z

		glm::mat4 p = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glBindVertexArray(_vao);
		for (unsigned int mip = 0; mip < numMips; ++mip)
		{
			for (unsigned int f = 0; f < 6; ++f)
			{
				glm::vec2 viewport = glm::vec2(dim) * static_cast<float>(std::pow(0.5f, mip));
				glViewport(0, 0, viewport.x, viewport.y);

				//! Update shader uniform variable
				float roughness = static_cast<float>(mip) / static_cast<float>(numMips - 1);
				shader->SendUniformVariable("roughness", roughness);
				shader->SendUniformVariable("mvp", p * mv[f]);

				//! Attach each face of the cube map to current bound framebuffer.
				glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0, texture, mip, f);
				
				//! Draw cube
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_INT, nullptr);
			}
		}
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SkyDome::IntegrateBRDF(unsigned int dim)
	{
		auto timerStart = std::chrono::high_resolution_clock::now();

		glCreateTextures(GL_TEXTURE_2D, 1, &_textureSet.brdfLUT);
		glTextureParameteri(_textureSet.brdfLUT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.brdfLUT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.brdfLUT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_textureSet.brdfLUT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//! Create framebuffer for capturing
		GLuint fbo;
		glCreateFramebuffers(1, &fbo);
		glTextureStorage2D(_textureSet.brdfLUT, 1, GL_RG16F, dim, dim);
		glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, _textureSet.brdfLUT, 0);

		if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "[SkyDome:IntegradeBRDF] Framebuffer is not complete\n";
			DebugUtils::PrintStack();
			return;
		}

		//! Create shader
		Shader shader;
		if (!shader.Initialize({
				{GL_VERTEX_SHADER, RESOURCES_DIR "shaders/integrate_brdf.vert"},
				{GL_FRAGMENT_SHADER, RESOURCES_DIR "shaders/integrate_brdf.frag"}
			}))
		{
			std::cerr << "[SkyDome:IntegradeBRDF] Failed to compile shader\n";
			DebugUtils::PrintStack();
			return;
		}
		shader.BindShaderProgram();
		shader.BindFragDataLocation("fragColor", 0);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, dim, dim);
		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbo);

		auto timerEnd = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration<double, std::milli>(timerEnd - timerStart).count();
		std::cout << "Intergrate BRDF LUT took " << elapsed << " (ms)\n";
	};

	void SkyDome::PrefilterDiffuse(unsigned int dim)
	{
		auto timerStart = std::chrono::high_resolution_clock::now();
		const unsigned int numMips = static_cast<unsigned int>(std::floor(std::log2(dim))) + 1;

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_textureSet.irradianceCube);
		glTextureParameteri(_textureSet.irradianceCube, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.irradianceCube, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.irradianceCube, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.irradianceCube, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_textureSet.irradianceCube, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(_textureSet.irradianceCube, numMips, GL_RGBA16F, dim, dim);

		//! Create framebuffer for capturing
		GLuint fbo;
		glCreateFramebuffers(1, &fbo);

		//! Create shader
		Shader shader;
		if (!shader.Initialize({ {GL_VERTEX_SHADER,   RESOURCES_DIR "shaders/filtercube.vert"		},
								 {GL_FRAGMENT_SHADER, RESOURCES_DIR "shaders/prefilter_diffuse.frag"} }))
		{
			std::cerr << "[SkyDome:PrefilterDiffuse] Failed to compile shader\n";
			DebugUtils::PrintStack();
			return;
		}
		shader.BindShaderProgram();
		shader.BindFragDataLocation("FragColor", 0);
		glBindTextureUnit(0, _textureSet.hdrTexture);
		glBindTextureUnit(1, _textureSet.accelTexture);
		RenderToCube(fbo, _textureSet.irradianceCube, &shader, dim, numMips);
		glDeleteFramebuffers(1, &fbo);

		auto timerEnd = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration<double, std::milli>(timerEnd - timerStart).count();
		std::cout << "Prefilter Diffuse took " << elapsed << " (ms)\n";
	}

	void SkyDome::PrefilterGlossy(unsigned int dim)
	{
		auto timerStart = std::chrono::high_resolution_clock::now();
		const unsigned int numMips = static_cast<unsigned int>(std::floor(std::log2(dim))) + 1;

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &_textureSet.prefilteredCube);
		glTextureParameteri(_textureSet.prefilteredCube, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.prefilteredCube, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.prefilteredCube, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(_textureSet.prefilteredCube, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTextureParameteri(_textureSet.prefilteredCube, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureStorage2D(_textureSet.prefilteredCube, numMips, GL_RGBA16F, dim, dim);

		//! Create framebuffer for capturing
		GLuint fbo;
		glCreateFramebuffers(1, &fbo);

		//! Create shader
		Shader shader;
		if (!shader.Initialize({ {GL_VERTEX_SHADER,   RESOURCES_DIR "shaders/filtercube.vert"		},
								 {GL_FRAGMENT_SHADER, RESOURCES_DIR "shaders/prefilter_glossy.frag"} }))
		{
			std::cerr << "[SkyDome:PrefilterGlossy] Failed to compile shader\n";
			DebugUtils::PrintStack();
			return;
		}
		shader.BindShaderProgram();
		shader.BindFragDataLocation("FragColor", 0);
		glBindTextureUnit(0, _textureSet.hdrTexture);
		glBindTextureUnit(1, _textureSet.accelTexture);
		RenderToCube(fbo, _textureSet.prefilteredCube, &shader, dim, numMips);
		glDeleteFramebuffers(1, &fbo);

		auto timerEnd = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration<double, std::milli>(timerEnd - timerStart).count();
		std::cout << "Prefilter Glossy took " << elapsed << " (ms)\n";
	}

	namespace //! Anonymous namespace for file-specific helper functions
	{
		struct EnvAccel
		{
			unsigned int alias{ 0 };
			float q{ 0.0f };
			float pdf{ 0.0f };
			float _padding{ 0.0f };
		};

		float BuildAliasMap(const std::vector<float>& data, std::vector<EnvAccel>& accel)
		{
			unsigned int size = static_cast<unsigned int>(data.size());
			//! Create qs (normalized)
			float sum = 0.0f;
			for (float d : data)
				sum += d;

			float fsize = static_cast<float>(data.size());
			for (unsigned int i = 0; i < data.size(); ++i)
				accel[i].q = fsize * data[i] / sum;

			//! Create partition table
			std::vector<unsigned int> partitionTable(size);
			unsigned int s = 0u, large = size;
			for (unsigned int i = 0; i < size; ++i)
				partitionTable[(accel[i].q < 1.0f) ? (s++) : (--large)] = accel[i].alias = i;

			//! Create Alias map
			for (s = 0; s < large && large < size; ++s)
			{
				const unsigned int j = partitionTable[s], k = partitionTable[large];
				accel[j].alias = k;
				accel[k].q += accel[j].q - 1.0f;
				large = (accel[k].q < 1.0f) ? (large + 1u) : large;
			}

			return sum;
		}
	}

	void SkyDome::CreateEnvironmentAccelTexture(const float* pixels, glm::vec2 size, GLuint accelTexture)
	{
		const unsigned int rx = size.x;
		const unsigned int ry = size.y;

		//! Create importance sampling data
		std::vector<EnvAccel> envAccel(rx * ry);
		std::vector<float> importanceData(rx * ry);
		float cosTheta0 = 1.0f;
		const float stepPhi = static_cast<float>(2.0f * glm::pi<float>()) / static_cast<float>(rx);
		const float stepTheta = glm::pi<float>() / static_cast<float>(ry);
		for (unsigned int y = 0; y < ry; ++y)
		{
			const float theta1 = static_cast<float>(y + 1) * stepTheta;
			const float cosTheta1 = std::cos(theta1);
			const float area = (cosTheta0 - cosTheta1) * stepPhi;
			cosTheta0 = cosTheta1;

			for (unsigned int x = 0; x < rx; ++x)
			{
				const unsigned int idx = y * rx + x;
				const unsigned int idx4 = idx * 4;
				importanceData[idx] = area * std::max(pixels[idx4], std::max(pixels[idx4 + 1], pixels[idx4 + 2]));
			}
		}

		const float invEnvIntegral = 1.0f / BuildAliasMap(importanceData, envAccel);
		for (unsigned int i = 0; i < rx * ry; ++i)
		{
			const unsigned int idx4 = i * 4;
			envAccel[i].pdf = std::max(pixels[idx4], std::max(pixels[idx4 + 1], pixels[idx4 + 2])) * invEnvIntegral;
		}

		glTextureStorage2D(accelTexture, 1, GL_RGBA32F, rx, ry);
		glTextureSubImage2D(accelTexture, 0, 0, 0, rx, ry, GL_RGBA, GL_FLOAT, envAccel.data());
	}
};
