#include <GL3/Scene.hpp>
#include <GL3/Shader.hpp>
#include <GL3/Texture.hpp>
#include <bitset>
#include <glad/glad.h>

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
		std::cout << "Loading Scene : " << filename << '\n';
		if (!Core::GLTFScene::Initialize(filename, format, [&](const tinygltf::Image& image) {
			std::string name = image.name.empty() ? std::string("texture") + std::to_string(this->_textures.size()) : image.name;
			std::cout << "Loading Image : " << name << '\n';
			auto texture = std::make_shared<GL3::Texture>();
			texture->Initialize(GL_TEXTURE_2D);
			texture->UploadTexture(&image.image[0], image.width, image.height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
			_textures.emplace(name, std::move(texture));
		}))
			return false;
		
		//! vertex buffer storages index
		int index = 0;
		//! Resize vertex buffer storage with number of vertex attribute type in given format,
		//! additionally index buffer also added.
		_buffers.resize(std::bitset<static_cast<unsigned int>(Core::VertexFormat::Last)>(
						static_cast<unsigned int>(format)).count() + 1);

		glCreateVertexArrays(1, &_vao);
		glCreateBuffers(_buffers.size(), _buffers.data());

		auto bindingBuffer = [&](void* data, size_t num, Core::VertexFormat attribute) {
			if (static_cast<int>(format & attribute))
			{
				const size_t numFloats = Core::VertexHelper::GetNumberOfFloats(attribute);
				const size_t stride = numFloats * sizeof(float);
				glNamedBufferStorage(_buffers[index], num * stride, data, GL_DYNAMIC_STORAGE_BIT);
				glVertexArrayVertexBuffer(_vao, index, _buffers[index], 0, stride);
				glEnableVertexArrayAttrib(_vao, index);
				glVertexArrayAttribFormat(_vao, index, numFloats, GL_FLOAT, GL_FALSE, 0);
				glVertexArrayAttribBinding(_vao, index, index);
				++index;
			}
		};

		bindingBuffer(_positions.data(), _positions.size(), Core::VertexFormat::Position3);
		bindingBuffer(_normals.data(),	 _normals.size(),	Core::VertexFormat::Normal3	 );
		bindingBuffer(_tangents.data(),  _tangents.size(),	Core::VertexFormat::Tangent4 );
		bindingBuffer(_colors.data(),	 _colors.size(),	Core::VertexFormat::Color4	 );
		bindingBuffer(_texCoords.data(), _texCoords.size(), Core::VertexFormat::TexCoord2);

		glCreateBuffers(1, &_ebo);
		glNamedBufferStorage(_ebo, _indices.size() * sizeof(unsigned int), _indices.data(), GL_DYNAMIC_STORAGE_BIT);
		glVertexArrayElementBuffer(_vao, _ebo);

		return true;
	}

	void Scene::Render(const std::shared_ptr< Shader >& shader, GLenum alphaMode)
	{
		(void)shader; (void)alphaMode;
	}

	void Scene::CleanUp()
	{
		_textures.clear();
		glDeleteBuffers(_buffers.size(), _buffers.data());
		glDeleteBuffers(1, &_ebo);
		glDeleteVertexArrays(1, &_vao);
	}

};