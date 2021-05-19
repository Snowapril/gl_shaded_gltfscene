#include <Core/GLTFScene.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_set>
#include <iostream>
#include <cassert>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION //! optional. disable exception handling.
#define TINYGLTF_ENABLE_DRACO
#define TINYGLTF_USE_CPP14

#if defined(_MSC_VER)
	#pragma warning(disable:4100)
	#pragma warning(disable:4804)
	#pragma warning(disable:4127)
	#pragma warning(disable:4018)
#elif (defined(__GNUC__) || defined(__GNUG__)) && !defined(__clang__)
	#pragma GCC diagnostic ignored "-Wbool-compare"
	#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include <tinygltf/tiny_gltf.h>

namespace Core {

	bool GLTFExtension::CheckRequiredExtensions(const tinygltf::Model& model)
	{
		static std::unordered_set<std::string> kSupportedExtensions{
			KHR_DARCO_MESH_EXTENSION_NAME,
			KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME,
			KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME,
			KHR_MATERIALS_PBR_SPECULAR_GLOSSINESS_EXTENSION_NAME,
			KHR_MATERIALS_SHEEN_EXTENSION_NAME,
			KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME,
			KHR_MATERIALS_UNLIT_EXTENSION_NAME,
			KHR_MATERIALS_VARIANTS_EXTENSION_NAME,
			KHR_MESH_QUANTIZATION_EXTENSION_NAME,
			KHR_TEXTURE_TRANSFORM_EXTENSION_NAME,
		};

		bool bSupported = true;
		for (auto& extension : model.extensionsRequired)
		{
			if (kSupportedExtensions.find(extension) == kSupportedExtensions.end())
			{
				std::cerr << "The extension " << extension << " is REQUIRED and not supproted" << std::endl;
				bSupported = false;
			}
		}

		return bSupported;
	}

	GLTFScene::GLTFScene()
	{
		//! Do nothing
	}

	GLTFScene::~GLTFScene()
	{
		//! Do nothing
	}

	bool GLTFScene::Initialize(const std::string& filename, VertexFormat format, ImageCallback imageCallback)
	{
		assert(static_cast<int>(format & Core::VertexFormat::Position3) && "Scene model must contain Position attribute");

		tinygltf::Model model;
		if (!LoadModel(&model, filename))
			return false;

		if (!GLTFExtension::CheckRequiredExtensions(model))
			return false;

		unsigned int numVertices{ 0 }, numIndices{ 0 }, primCount{ 0 }, meshCount{ 0 };
		for (const auto& mesh : model.meshes)
		{
			std::vector<unsigned int> vPrim;
			for (const auto& prim : mesh.primitives)
			{
				if (prim.mode != TINYGLTF_MODE_TRIANGLES)
					continue;

				const auto& posAccessor = model.accessors[prim.attributes.find("POSITION")->second];
				numVertices += posAccessor.count;
				if (prim.indices > -1)
				{
					const auto& indexAccessor = model.accessors[prim.indices];
					numIndices += indexAccessor.count;
				}
				else
				{
					numIndices += posAccessor.count;
				}
				vPrim.push_back(primCount++);
			}
			_meshToPrimMap[meshCount++] = std::move(vPrim);
		}

		_positions.reserve(numVertices);
		_indices.reserve(numIndices);
		if (static_cast<int>(format & VertexFormat::Normal3))
			_normals.reserve(numVertices);
		if (static_cast<int>(format & VertexFormat::Tangent4))
			_tangents.reserve(numVertices);
		if (static_cast<int>(format & VertexFormat::Color4))
			_colors.reserve(numVertices);
		if (static_cast<int>(format & VertexFormat::TexCoord2))
			_texCoords.reserve(numVertices);

		//! Convert all mesh/primitves+ to a single primitive per mesh.
		for (const auto& mesh : model.meshes)
		{
			for (const auto& prim : mesh.primitives)
			{
				ProcessMesh(model, prim, format, mesh.name);
			}
		}

		//! Transforming the scene hierarchy to a flat list.
		int defaultScene = model.defaultScene > -1 ? model.defaultScene : 0;
		const auto& scene = model.scenes[defaultScene];
		for (auto nodeIdx : scene.nodes)
		{
			ProcessNode(model, nodeIdx, glm::mat4(1.0f));
		}

		//! Compute scene dimension
		CalculateSceneDimension();
		ComputeCamera();

		//! Clear all temporal resources.
		_meshToPrimMap.clear();
		_u8Buffer.clear();
		_u16Buffer.clear();
		_u32Buffer.clear();

		//! Import materials from the model
		ImportMaterials(model);

		//! Finally import images from the model
		if (imageCallback != nullptr)
		{
			for (const auto& image : model.images)
				imageCallback(image);
		}

		return true;
	}

	void GLTFScene::ProcessMesh(const tinygltf::Model& model, const tinygltf::Primitive& mesh, VertexFormat format, const std::string& name)
	{
		GLTFPrimMesh resultMesh;
		resultMesh.name = name;
		resultMesh.materialIndex = mesh.material < 0 ? 0 : mesh.material;
		resultMesh.vertexOffset = static_cast<unsigned int>(_positions.size());
		resultMesh.firstIndex = static_cast<unsigned int>(_indices.size());

		//! Only triangles supported.
		if (mesh.mode != TINYGLTF_MODE_TRIANGLES)
			return;

		//! Indices
		if (mesh.indices > -1)
		{
			const tinygltf::Accessor& indexAccessor = model.accessors[mesh.indices];
			const tinygltf::BufferView& bufferView = model.bufferViews[indexAccessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

			resultMesh.indexCount = static_cast<unsigned int>(indexAccessor.count);
			switch (indexAccessor.componentType)
			{
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
				_u32Buffer.resize(indexAccessor.count);
				std::memcpy(&_u32Buffer[0], &buffer.data[indexAccessor.byteOffset + bufferView.byteOffset], indexAccessor.count * sizeof(unsigned int));
				_indices.insert(_indices.end(), std::make_move_iterator(_u32Buffer.begin()), std::make_move_iterator(_u32Buffer.end()));
				break;
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
				_u16Buffer.resize(indexAccessor.count);
				std::memcpy(&_u16Buffer[0], &buffer.data[indexAccessor.byteOffset + bufferView.byteOffset], indexAccessor.count * sizeof(unsigned short));
				_indices.insert(_indices.end(), std::make_move_iterator(_u16Buffer.begin()), std::make_move_iterator(_u16Buffer.end()));
				break;
			case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
				_u8Buffer.resize(indexAccessor.count);
				std::memcpy(&_u8Buffer[0], &buffer.data[indexAccessor.byteOffset + bufferView.byteOffset], indexAccessor.count * sizeof(unsigned char));
				_indices.insert(_indices.end(), std::make_move_iterator(_u8Buffer.begin()), std::make_move_iterator(_u8Buffer.end()));
				break;
			default:
				std::cerr << "Unknown index component type : " << indexAccessor.componentType << " is not supported" << std::endl;
				return;
			}
		}
		else
		{
			//! Primitive without indices, creating them
			const auto& accessor = model.accessors[mesh.attributes.find("POSITION")->second];
			for (unsigned int i = 0; i < accessor.count; ++i)
				_indices.push_back(i);
			resultMesh.indexCount = static_cast<unsigned int>(accessor.count);
		}

		//! POSITION
		{
			[[maybe_unused]] bool result = GetAttributes<glm::vec3>(model, mesh, _positions, "POSITION");

			//! Keeping the size of this primitive (spec says this is required information)
			const auto& accessor = model.accessors[mesh.attributes.find("POSITION")->second];
			resultMesh.vertexCount = static_cast<unsigned int>(accessor.count);
			if (accessor.minValues.empty() == false)
				resultMesh.min = glm::vec3(accessor.minValues[0], accessor.minValues[1], accessor.minValues[2]);
			if (accessor.maxValues.empty() == false)
				resultMesh.max = glm::vec3(accessor.maxValues[0], accessor.maxValues[1], accessor.maxValues[2]);
		}

		//! NORMAL
		if (static_cast<int>(format & VertexFormat::Normal3))
		{
			if (!GetAttributes<glm::vec3>(model, mesh, _normals, "NORMAL"))
			{
				//! You need to compute the normals
				std::vector<glm::vec3> meshNormals(resultMesh.vertexCount, glm::vec3(0.0f));
				for (size_t i = 0; i < resultMesh.indexCount; i += 3)
				{
					unsigned int idx0 = _indices[resultMesh.firstIndex + i + 0];
					unsigned int idx1 = _indices[resultMesh.firstIndex + i + 1];
					unsigned int idx2 = _indices[resultMesh.firstIndex + i + 2];
					const auto& pos0 = _positions[resultMesh.vertexOffset + idx0];
					const auto& pos1 = _positions[resultMesh.vertexOffset + idx1];
					const auto& pos2 = _positions[resultMesh.vertexOffset + idx2];
					const auto edge0 = glm::normalize(pos1 - pos0);
					const auto edge1 = glm::normalize(pos2 - pos0);
					const auto n = glm::normalize(glm::cross(edge0, edge1));
					meshNormals[idx0] += n;
					meshNormals[idx1] += n;
					meshNormals[idx2] += n;
				}
				_normals.insert(_normals.end(), std::make_move_iterator(meshNormals.begin()), std::make_move_iterator(meshNormals.end()));
			}
		}

		//! TEXCOORD2
		if (static_cast<int>(format & VertexFormat::TexCoord2))
		{
			if (!GetAttributes<glm::vec2>(model, mesh, _texCoords, "TEXCOORD_0"))
			{
				//! CubeMap projection
				for (unsigned int i = 0; i < resultMesh.vertexCount; ++i)
				{
					const auto& pos = _positions[resultMesh.vertexOffset + i];
					float absX = std::fabs(pos.x);
					float absY = std::fabs(pos.y);
					float absZ = std::fabs(pos.z);

					int isXPositive = pos.x > 0.0f ? 1 : 0;
					int isYPositive = pos.y > 0.0f ? 1 : 0;
					int isZPositive = pos.z > 0.0f ? 1 : 0;

					float mapAxis{ 0.0f }, uc{ 0.0f }, vc{ 0.0f };
					//! Positive X
					if (isXPositive && absX >= absY && absX >= absZ)
					{
						//! u(0~1) goes from +z to -z
						//! v(0~1) goes from -y to +y
						mapAxis = absX;
						uc = -pos.z;
						vc = pos.y;
					}
					//! Negative X
					if (!isXPositive && absX >= absY && absX >= absZ)
					{
						//! u(0~1) goes from -z to +z
						//! v(0~1) goes from -y to +y
						mapAxis = absX;
						uc = pos.z;
						vc = pos.y;
					}
					//! Positive Y
					if (isYPositive && absY >= absX && absY >= absZ)
					{
						//! u(0~1) goes from -x to +x
						//! v(0~1) goes from +z to -z
						mapAxis = absY;
						uc = pos.x;
						vc = -pos.z;
					}
					//! Negative Y
					if (!isYPositive && absY >= absX && absY >= absZ)
					{
						//! u(0~1) goes from -x to +x
						//! v(0~1) goes from -z to +z
						mapAxis = absY;
						uc = pos.x;
						vc = pos.z;
					}
					//! Positive Z
					if (isZPositive && absY >= absX && absY >= absZ)
					{
						//! u(0~1) goes from -x to +x
						//! v(0~1) goes from -y to +y
						mapAxis = absZ;
						uc = pos.x;
						vc = pos.y;
					}
					//! Negative Z
					if (!isZPositive && absZ >= absX && absZ >= absY)
					{
						//! u(0~1) goes from +x to -x
						//! v(0~1) goes from -y to +y
						mapAxis = absZ;
						uc = -pos.x;
						vc = pos.y;
					}

					//! Convert range from (-1~1) into (0~1)
					float u = (uc / mapAxis + 1.0f) * 0.5f;
					float v = (vc / mapAxis + 1.0f) * 0.5f;

					_texCoords.push_back(glm::vec2(u, v));
				}
			}
		}

		//! TANGENT
		if (static_cast<int>(format & VertexFormat::Tangent4))
		{
			if (!GetAttributes(model, mesh, _tangents, "TANGENT"))
			{
				//! Implementation in "Foundations of Game Engine Development : Volume2 Rendering"
				std::vector<glm::vec3> tangents(resultMesh.vertexCount, glm::vec3(0.0f));
				std::vector<glm::vec3> bitangents(resultMesh.vertexCount, glm::vec3(0.0f));
				for (size_t i = 0; i < resultMesh.indexCount; i += 3)
				{
					//! Local index
					unsigned int idx0 = _indices[resultMesh.firstIndex + i + 0];
					unsigned int idx1 = _indices[resultMesh.firstIndex + i + 1];
					unsigned int idx2 = _indices[resultMesh.firstIndex + i + 2];
					//! Global index
					unsigned int gidx0 = idx0 + resultMesh.vertexOffset;
					unsigned int gidx1 = idx1 + resultMesh.vertexOffset;
					unsigned int gidx2 = idx2 + resultMesh.vertexOffset;

					const auto& pos0 = _positions[gidx0];
					const auto& pos1 = _positions[gidx1];
					const auto& pos2 = _positions[gidx2];

					const auto& uv0 = _texCoords[gidx0];
					const auto& uv1 = _texCoords[gidx1];
					const auto& uv2 = _texCoords[gidx2];

					glm::vec3 e1 = pos1 - pos0, e2 = pos2 - pos0;
					float x1 = uv1.x - uv0.x, x2 = uv2.x - uv0.x;
					float y1 = uv1.y - uv0.y, y2 = uv2.y - uv0.y;

					const float r = 1.0f / (x1 * y2 - x2 * y1);
					glm::vec3 tangent = (e1 * y2 - e2 * y1) * r;
					glm::vec3 bitangent = (e2 * x1 - e1 * x2) * r;

					//! In case of degenerated UV coordinates
					if (x1 == 0 || x2 == 0 || y1 == 0 || y2 == 0)
					{
						const auto& nrm0 = _normals[gidx0];
						const auto& nrm1 = _normals[gidx1];
						const auto& nrm2 = _normals[gidx2];
						const auto N = (nrm0 + nrm1 + nrm2) / glm::vec3(3.0f);

						if (std::abs(N.x) > std::abs(N.y))
							tangent = glm::vec3(N.z, 0, -N.x) / std::sqrt(N.x * N.x + N.z * N.z);
						else
							tangent = glm::vec3(0, -N.z, N.y) / std::sqrt(N.y * N.y + N.z * N.z);
						bitangent = glm::cross(N, tangent);
					}

					tangents[idx0] += tangent;
					tangents[idx1] += tangent;
					tangents[idx2] += tangent;
					bitangents[idx0] += bitangent;
					bitangents[idx1] += bitangent;
					bitangents[idx2] += bitangent;
				}

				for (unsigned int i = 0; i < resultMesh.vertexCount; ++i)
				{
					const auto& n = _normals[resultMesh.vertexOffset + i];
					const auto& t = tangents[i];
					const auto& b = bitangents[i];

					//! Gram schmidt orthogonalize
					glm::vec3 tangent = glm::normalize(t - n * glm::vec3(glm::dot(n, t)));
					//! Calculate the handedness
					float handedness = (glm::dot(glm::cross(t, b), n) > 0.0f) ? 1.0f : -1.0f;
					_tangents.emplace_back(tangent.x, tangent.y, tangent.z, handedness);
				}
			}
		}

		//! COLOR
		if (static_cast<int>(format & VertexFormat::Color4))
		{
			if (!GetAttributes(model, mesh, _colors, "COLOR_0"))
			{
				_colors.insert(_colors.end(), resultMesh.vertexCount, glm::vec4(1.0f));
			}
		}

		_scenePrimMeshes.emplace_back(resultMesh);
	}

	bool GLTFScene::LoadModel(tinygltf::Model* model, const std::string& filename)
	{
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		bool res = loader.LoadBinaryFromFile(model, &err, &warn, filename);
		if (!res)
			res = loader.LoadASCIIFromFile(model, &err, &warn, filename);

		if (!res)
			std::cerr << "Failed to load GLTF model : " << filename << std::endl;

		return res;
	}

	void GLTFScene::ProcessNode(const tinygltf::Model& model, int& nodeIdx, const glm::mat4& parentMatrix)
	{
		const auto& node = model.nodes[nodeIdx];

		glm::mat4 localMat = GetLocalMatrix(node);
		glm::mat4 worldMat = parentMatrix * localMat;

		if (node.mesh > -1)
		{
			const auto& meshes = _meshToPrimMap[node.mesh];
			for (const auto& mesh : meshes)
			{
				GLTFNode tempNode;
				tempNode.primMesh = mesh;
				tempNode.world = worldMat;
				_sceneNodes.emplace_back(tempNode);
			}
		}
		else if (node.camera > -1)
		{
			GLTFCamera camera;
			camera.world = worldMat;
			camera.camera = model.cameras[model.nodes[nodeIdx].camera];

			//! If the node has the Iray extension, extract the camera information
			if (node.extensions.find("NV_attributes_iray") != node.extensions.end())
			{
				auto& iray_ext = node.extensions.find("NV_attributes_iray")->second;
				auto& attributes = iray_ext.Get("attributes");
				for (size_t idx = 0; idx < attributes.ArrayLen(); ++idx)
				{
					auto& attrib = attributes.Get(idx);
					std::string attName = attrib.Get("name").Get<std::string>();
					auto& attValue = attrib.Get("value");
					if (attValue.IsArray())
					{
						auto vec = GetVector<float>(attValue);
						if (attName == "ivew:position")
							camera.eye = { vec[0], vec[1], vec[2] };
						else if (attName == "iview:interest")
							camera.center = { vec[0], vec[1], vec[2] };
						else if (attName == "iview:up")
							camera.up = { vec[0], vec[1], vec[2] };
					}
				}
			}

			_sceneCameras.emplace_back(camera);
		}
		else if (node.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME) != node.extensions.end())
		{
			GLTFLight light;
			const auto& ext = node.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME)->second;
			auto lightIdx = ext.Get("light").GetNumberAsInt();
			light.light = model.lights[lightIdx];
			light.world = worldMat;
			_sceneLights.emplace_back(light);
		}

		for (auto child : node.children)
		{
			ProcessNode(model, child, worldMat);
		}
	}

	glm::mat4 GLTFScene::GetLocalMatrix(const tinygltf::Node& node)
	{
		glm::mat4 T(1.0f);
		glm::mat4 S(1.0f);
		glm::mat4 R(1.0f);
		glm::mat4 nodeMat(1.0f);

		if (node.translation.empty() == false)
			T = glm::translate(T, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
		if (node.scale.empty() == false)
			S = glm::scale(S, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
		if (node.rotation.empty() == false)
		{
			glm::quat rotationQuat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			R = glm::toMat4(rotationQuat);
		}
		if (node.matrix.empty() == false)
		{
			float* nodeMatPtr = glm::value_ptr(nodeMat);
			for (int i = 0; i < 16; ++i)
				nodeMatPtr[i] = static_cast<float>(node.matrix[i]);
		}
		return T * R * S * nodeMat;
	}

	void GLTFScene::CalculateSceneDimension()
	{
		auto bbMin = glm::vec3(std::numeric_limits<float>::max());
		auto bbMax = glm::vec3(std::numeric_limits<float>::min());
		for (const auto& node : _sceneNodes)
		{
			const auto& mesh = _scenePrimMeshes[node.primMesh];

			auto localMin = node.world * glm::vec4(mesh.min, 1.0f);
			auto localMax = node.world * glm::vec4(mesh.max, 1.0f);

			bbMin = { std::min(bbMin.x, localMin.x), std::min(bbMin.z, localMin.z), std::min(bbMin.z, localMin.z) };
			bbMax = { std::max(bbMax.x, localMax.x), std::max(bbMax.z, localMax.z), std::max(bbMax.z, localMax.z) };
		}

		if (bbMin == bbMax)
		{
			bbMin = glm::vec3(-1.0f);
			bbMax = glm::vec3(1.0f);
		}

		_sceneDim.min = bbMin;
		_sceneDim.max = bbMax;
		_sceneDim.size = bbMax - bbMin;
		_sceneDim.center = (bbMin + bbMax) * 0.5f;
		_sceneDim.radius = glm::length(bbMax - bbMin) * 0.5f;
	}

	void GLTFScene::ComputeCamera()
	{
		for (auto& camera : _sceneCameras)
		{
			if (camera.eye == camera.center)
			{
				camera.eye = camera.world * glm::vec4(camera.eye, 1.0f);
				float distance = glm::length(_sceneDim.center - camera.eye);
				glm::mat3 rot(camera.world);
				camera.center = { 0.0f, 0.0f, -distance };
				camera.center = camera.eye + (rot * camera.center);
				camera.up = { 0.0f, 1.0f, 0.0f };
			}
		}
	}

	void GLTFScene::ImportMaterials(const tinygltf::Model& model)
	{
		_sceneMaterials.reserve(model.materials.size());

		for (const auto& mat : model.materials)
		{
			GLTFMaterial material;
			material.alphaCutoff = static_cast<float>(mat.alphaCutoff);
			material.alphaMode = mat.alphaMode == "MASK" ? 1 : (mat.alphaMode == "BLEND" ? 2 : 0);
			material.doubleSided = mat.doubleSided ? 1 : 0;
			material.emissiveFactor = glm::vec3(mat.emissiveFactor[0], mat.emissiveFactor[1], mat.emissiveFactor[2]);
			material.emissiveTexture = mat.emissiveTexture.index;
			material.normalTexture = mat.normalTexture.index;
			material.normalTextureScale = static_cast<float>(mat.normalTexture.scale);
			material.occlusionTexture = mat.occlusionTexture.index;
			material.occlusionTextureStrength = static_cast<float>(mat.occlusionTexture.strength);

			//! PBR Metallic roughness
			auto& pbr = mat.pbrMetallicRoughness;
			material.baseColorFactor = glm::vec4(pbr.baseColorFactor[0], pbr.baseColorFactor[1], pbr.baseColorFactor[2], pbr.baseColorFactor[3]);
			material.baseColorTexture = pbr.baseColorTexture.index;
			material.metallicFactor = static_cast<float>(pbr.metallicFactor);
			material.metallicRoughnessTexture = pbr.metallicRoughnessTexture.index;
			material.roughnessFactor = static_cast<float>(pbr.roughnessFactor);

			//! KHR_materials_pbrSpecularGlossiness
			if (mat.extensions.find(KHR_MATERIALS_PBR_SPECULAR_GLOSSINESS_EXTENSION_NAME) != mat.extensions.end())
			{
				material.shadingModel = 1;

				const auto& ext = mat.extensions.find(KHR_MATERIALS_PBR_SPECULAR_GLOSSINESS_EXTENSION_NAME)->second;
				GetValue<glm::vec4>(ext, "diffuseFactor", material.specularGlossiness.diffuseFactor);
				GetValue<float>(ext, "glossinessFactor", material.specularGlossiness.glossinessFactor);
				GetValue<glm::vec3>(ext, "specularFactor", material.specularGlossiness.specularFactor);
				GetTextureID(ext, "diffuseTexture", material.specularGlossiness.diffuseTexture);
				GetTextureID(ext, "specularGlossinessTexture", material.specularGlossiness.specularGlossinessTexture);
			}

			// KHR_texture_transform
			if (pbr.baseColorTexture.extensions.find(KHR_TEXTURE_TRANSFORM_EXTENSION_NAME) != pbr.baseColorTexture.extensions.end())
			{
				const auto& ext = pbr.baseColorTexture.extensions.find(KHR_TEXTURE_TRANSFORM_EXTENSION_NAME)->second;
				auto& tt = material.textureTransform;
				GetValue<glm::vec2>(ext, "offset", tt.offset);
				GetValue<glm::vec2>(ext, "scale", tt.scale);
				GetValue<float>(ext, "rotation", tt.rotation);
				GetValue<int>(ext, "texCoord", tt.texCoord);

				// Computing the transformation
				glm::mat3 translation = glm::mat3(1, 0, tt.offset.x, 0, 1, tt.offset.y, 0, 0, 1);
				glm::mat3 rotation = glm::mat3(cos(tt.rotation), sin(tt.rotation), 0, -sin(tt.rotation), cos(tt.rotation), 0, 0, 0, 1);
				glm::mat3 scale = glm::mat3(tt.scale.x, 0, 0, 0, tt.scale.y, 0, 0, 0, 1);
				tt.uvTransform = scale * rotation * translation;
			}

			// KHR_materials_unlit
			if (mat.extensions.find(KHR_MATERIALS_UNLIT_EXTENSION_NAME) != mat.extensions.end())
			{
				material.unlit.active = 1;
			}

			// KHR_materials_clearcoat
			if (mat.extensions.find(KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME) != mat.extensions.end())
			{
				const auto& ext = mat.extensions.find(KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME)->second;
				GetValue<float>(ext, "clearcoatFactor", material.clearcoat.factor);
				GetTextureID(ext, "clearcoatTexture", material.clearcoat.texture);
				GetValue<float>(ext, "clearcoatRoughnessFactor", material.clearcoat.roughnessFactor);
				GetTextureID(ext, "clearcoatRoughnessTexture", material.clearcoat.roughnessTexture);
				GetTextureID(ext, "clearcoatNormalTexture", material.clearcoat.normalTexture);
			}

			// KHR_materials_sheen
			if (mat.extensions.find(KHR_MATERIALS_SHEEN_EXTENSION_NAME) != mat.extensions.end())
			{
				const auto& ext = mat.extensions.find(KHR_MATERIALS_SHEEN_EXTENSION_NAME)->second;
				GetValue<glm::vec3>(ext, "sheenColorFactor", material.sheen.colorFactor);
				GetTextureID(ext, "sheenColorTexture", material.sheen.colorTexture);
				GetValue<float>(ext, "sheenRoughnessFactor", material.sheen.roughnessFactor);
				GetTextureID(ext, "sheenRoughnessTexture", material.sheen.roughnessTexture);
			}

			// KHR_materials_transmission
			if (mat.extensions.find(KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME) != mat.extensions.end())
			{
				const auto& ext = mat.extensions.find(KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME)->second;
				GetValue<float>(ext, "transmissionFactor", material.transmission.factor);
				GetTextureID(ext, "transmissionTexture", material.transmission.texture);
			}

			_sceneMaterials.emplace_back(material);
		}
	}

	void GLTFScene::ReleaseSourceData()
	{
		_positions.clear();
		_normals.clear();
		_tangents.clear();
		_colors.clear();
		_texCoords.clear();
		_indices.clear();
	}
};