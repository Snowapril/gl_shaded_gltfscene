#include <GL3/Scene.hpp>
#include <GL3/Shader.hpp>
#include <Core/Macros.hpp>
#include <glad/glad.h>
#include <bitset>
#include <algorithm>
#include <chrono>

using namespace glm;
#include <shaders/gltf.glsl>

namespace GL3 {
	Scene::Scene()
	{
		//! Do nothing
	}

	Scene::~Scene()
	{
		//! Do nothing
	}

	bool Scene::Initialize(const std::string& filename, Core::VertexFormat format)
	{
		auto timerStart = std::chrono::high_resolution_clock::now();

		if (!Core::GLTFScene::Initialize(filename, format, [&](const tinygltf::Image& image) {
			std::string name = image.name.empty() ? std::string("texture") + std::to_string(this->_textures.size()) : image.name;
			GLuint texture;
			glCreateTextures(GL_TEXTURE_2D, 1, &texture);
			glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTextureStorage2D(texture, 1, GL_RGBA8, image.width, image.height);
			glTextureSubImage2D(texture, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, &image.image[0]);
			glGenerateTextureMipmap(texture);
			_debug.SetObjectName(GL_TEXTURE, texture, name);
			_textures.emplace_back(texture);
		}))
			return false;
		
		auto timerEnd = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration<double, std::milli>(timerEnd - timerStart).count();
		std::cout << "Loading Scene " << filename << " took " << elapsed << " (ms)\n";

		//! vertex buffer storages index
		int index = 0;

		//! Resize vertex buffer storage with number of vertex attribute type in given format,
		//! additionally index buffer also added.
		_buffers.resize(std::bitset<static_cast<unsigned int>(Core::VertexFormat::Last)>(
						static_cast<unsigned int>(format)).count() + 1);

		//! Create & Bind vertex array object
		glCreateVertexArrays(1, &_vao);
		_debug.SetObjectName(GL_VERTEX_ARRAY, _vao, "Scene Vertex Array Object");
		glCreateBuffers(_buffers.size(), _buffers.data());

		//! Temporary buffer binding lambda function
		auto bindingBuffer = [&](void* data, size_t num, Core::VertexFormat attribute) {
			if (static_cast<int>(format & attribute))
			{
				const size_t numFloats = Core::VertexHelper::GetNumberOfFloats(attribute);
				const size_t stride = numFloats * sizeof(float);
				glNamedBufferStorage(_buffers[index], num * stride, data, GL_MAP_READ_BIT);
				glVertexArrayVertexBuffer(_vao, index, _buffers[index], 0, stride);
				glEnableVertexArrayAttrib(_vao, index);
				glVertexArrayAttribFormat(_vao, index, numFloats, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_vao, index, index);
				_debug.SetObjectName(GL_BUFFER, _buffers[index], "Scene Buffer #" + std::to_string(index));
				++index;
			}
		};

		//! Create & Bind the vertex buffers
		bindingBuffer(_positions.data(), _positions.size(), Core::VertexFormat::Position3);
		bindingBuffer(_normals.data(),	 _normals.size(),	Core::VertexFormat::Normal3	 );
		bindingBuffer(_tangents.data(),  _tangents.size(),	Core::VertexFormat::Tangent4 );
		bindingBuffer(_colors.data(),	 _colors.size(),	Core::VertexFormat::Color4	 );
		bindingBuffer(_texCoords.data(), _texCoords.size(), Core::VertexFormat::TexCoord2);

		//! Create buffers for indices
		glCreateBuffers(1, &_ebo);
		glNamedBufferStorage(_ebo, _indices.size() * sizeof(unsigned int), _indices.data(), GL_MAP_READ_BIT);
		glVertexArrayElementBuffer(_vao, _ebo);
		_debug.SetObjectName(GL_BUFFER, _ebo, "Scene Element Buffer");

		//! Create shader storage buffer object for matrices of scene nodes
		const size_t numMatrices = std::count_if(_sceneNodes.begin(), _sceneNodes.end(), [](const GLTFNode& node){
			return !node.primMeshes.empty();
		});
		glGenBuffers(1, &_matrixBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _matrixBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, numMatrices * sizeof(NodeMatrix), nullptr, GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _matrixBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		_debug.SetObjectName(GL_BUFFER, _matrixBuffer, "Scene Instance Buffer");

		//! Initialize matrix buffer contents
		UpdateMatrixBuffer();

		//! Create shader storage buffer object for materials and fill it
		std::vector<GltfShadeMaterial> materials;
		materials.reserve(_sceneMaterials.size());
		for (const auto& material : _sceneMaterials)
		{
			materials.push_back({ material.baseColorFactor,
								  material.baseColorTexture,
								  material.metallicFactor,
								  material.roughnessFactor,
								  material.metallicRoughnessTexture,
								  material.specularGlossiness.diffuseFactor,
								  material.specularGlossiness.specularFactor,
								  material.specularGlossiness.diffuseTexture,
								  material.specularGlossiness.glossinessFactor,
								  material.specularGlossiness.specularGlossinessTexture,
								  material.emissiveTexture,
								  material.alphaMode,
								  material.emissiveFactor,
								  material.alphaCutoff,
								  material.doubleSided,
								  material.normalTexture,
								  material.normalTextureScale,
								  material.occlusionTexture,
								  material.occlusionTextureStrength,
								  material.shadingModel});
		}
		glGenBuffers(1, &_materialBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, materials.size() * sizeof(GltfShadeMaterial), materials.data(), GL_STATIC_COPY);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _materialBuffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		_debug.SetObjectName(GL_BUFFER, _materialBuffer, "Scene Material Buffer");

		//! After uploading all required vertex data, We can release them to free
		ReleaseSourceData();

		return true;
	}

	void Scene::Update(double dt)
	{
		bool sceneModified = UpdateAnimation(_animIndex, _timeElapsed);

		//! If the scene is modified, update the matrix buffer
		if (sceneModified)
			UpdateMatrixBuffer();			

		_timeElapsed += dt;
	}

	void Scene::Render(const std::shared_ptr< Shader >& shader, GLenum alphaMode) const
	{
		UNUSED_VARIABLE(alphaMode);

		auto scope = _debug.ScopeLabel("Scene Rendering");
		glBindVertexArray(_vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _matrixBuffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _materialBuffer);

		//! Use block-scope for calling destructor of scope label instance
		{
			auto textureScope = _debug.ScopeLabel("Scene Texture Binding");
			for (int i = 0; i < static_cast<int>(_textures.size()); ++i)
				glBindTextureUnit(i + 3, _textures[i]);
		}

		int lastMaterialIdx = -1, instanceIdx = 0;
		for (auto& node : _sceneNodes)
		{
			shader->SendUniformVariable("instanceIdx", instanceIdx);

			for (unsigned int meshIdx : node.primMeshes)
			{
				auto& primMesh = _scenePrimMeshes[meshIdx];
				if (primMesh.materialIndex != lastMaterialIdx)
				{
					auto materialScope = _debug.ScopeLabel("Material Binding: " + std::to_string(instanceIdx));
					shader->SendUniformVariable("materialIdx", primMesh.materialIndex);
					lastMaterialIdx = primMesh.materialIndex;
				}

				auto drawScope = _debug.ScopeLabel("Draw Mesh: " + std::to_string(instanceIdx));
				//! Draw elements with primitive mesh index informations.
				glDrawElementsBaseVertex(GL_TRIANGLES, primMesh.indexCount, GL_UNSIGNED_INT,
					reinterpret_cast<const void*>(primMesh.firstIndex * sizeof(unsigned int)), primMesh.vertexOffset);

				++instanceIdx;
			}
		}

		glBindVertexArray(0);
	}

	void Scene::UpdateMatrixBuffer()
	{
		std::vector<NodeMatrix> matrices;
		matrices.reserve(_sceneNodes.size());
		for (const auto& node : _sceneNodes)
		{
			if (!node.primMeshes.empty())
			{
				NodeMatrix instance;
				instance.first = node.world;
				if (glm::determinant(instance.first) == 0.0f)
					instance.second = glm::transpose(instance.first);
				else
					instance.second = glm::transpose(glm::inverse(instance.first));
				matrices.emplace_back(std::move(instance));
			}
		}

		//! TODO(snowapril) : mark only modified node and update the contents of them
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, _matrixBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, matrices.size() * sizeof(NodeMatrix), matrices.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	void Scene::CleanUp()
	{
		glDeleteTextures(_textures.size(), _textures.data());
		glDeleteBuffers(1, &_matrixBuffer);
		glDeleteBuffers(1, &_materialBuffer);
		glDeleteBuffers(_buffers.size(), _buffers.data());
		glDeleteBuffers(1, &_ebo);
		glDeleteVertexArrays(1, &_vao);
	}

	size_t Scene::GetNumAnimations() const
	{
		return _sceneAnims.size();
	}

	void Scene::SetAnimIndex(size_t animIndex)
	{
		_animIndex = animIndex;
	}
};