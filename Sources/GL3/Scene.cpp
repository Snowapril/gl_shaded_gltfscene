#include <GL3/Scene.hpp>
#include <GL3/Shader.hpp>
#include <GL3/Texture.hpp>

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
		
		return true;
	}

	void Scene::CleanUp()
	{
		_textures.clear();
	}

};