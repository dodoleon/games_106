#include "assimp_helper.h"

#include <assimp/DefaultLogger.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <igl/edge_flaps.h>

#include <filesystem>
#include <iostream>

static Assimp::Importer s_importer;
static const aiScene   *s_scene;

inline Eigen::Matrix4d convert_matrix(const aiMatrix4x4 &transform)
{
	Eigen::Matrix4d ret;
	ret(0, 0) = transform.a1;
	ret(0, 1) = transform.b1;
	ret(0, 2) = transform.c1;
	ret(0, 3) = transform.d1;
	ret(1, 0) = transform.a2;
	ret(1, 1) = transform.b2;
	ret(1, 2) = transform.c2;
	ret(1, 3) = transform.d2;
	ret(2, 0) = transform.a3;
	ret(2, 1) = transform.b3;
	ret(2, 2) = transform.c3;
	ret(2, 3) = transform.d3;
	ret(3, 0) = transform.a4;
	ret(3, 1) = transform.b4;
	ret(3, 2) = transform.c4;
	ret(3, 3) = transform.d4;
	return ret;
}

Scene AssimpHelper::Load(const char *filename, uint32_t flags)
{
	Scene scene;

	scene.num_instance   = 0;
	scene.num_mesh       = 0;
	scene.num_total_mesh = 0;

	std::map<std::string, int32_t> texture_map;

	if (s_scene = s_importer.ReadFile(filename, flags | aiProcess_FlipUVs))
	{
		scene.filename     = filename;
		scene.assimp_scene = s_scene;

		// Handle submesh
		scene.meshes.resize(scene.assimp_scene->mNumMeshes);
		for (uint32_t i = 0; i < scene.assimp_scene->mNumMeshes; i++)
		{
			scene.num_mesh++;

			aiMesh *assimp_mesh = scene.assimp_scene->mMeshes[i];

			const uint32_t num_vertices = assimp_mesh->mNumVertices;
			const uint32_t num_faces    = assimp_mesh->mNumFaces;

			scene.meshes[i].V.resize(num_vertices, 3);
			scene.meshes[i].N.resize(num_vertices, 3);
			scene.meshes[i].UV.resize(num_vertices, 2);
			scene.meshes[i].F.resize(num_faces, 3);

			// Read vertices
			for (uint32_t j = 0; j < num_vertices; j++)
			{
				scene.meshes[i].V(j, 0) = assimp_mesh->mVertices[j].x;
				scene.meshes[i].V(j, 1) = assimp_mesh->mVertices[j].y;
				scene.meshes[i].V(j, 2) = assimp_mesh->mVertices[j].z;

				scene.meshes[i].N(j, 0) = assimp_mesh->mNormals ? assimp_mesh->mNormals[j].x : 0.f;
				scene.meshes[i].N(j, 1) = assimp_mesh->mNormals ? assimp_mesh->mNormals[j].y : 0.f;
				scene.meshes[i].N(j, 2) = assimp_mesh->mNormals ? assimp_mesh->mNormals[j].z : 0.f;

				scene.meshes[i].UV(j, 0) = assimp_mesh->mTextureCoords[0] ? assimp_mesh->mTextureCoords[0][j].x : 0.f;
				scene.meshes[i].UV(j, 1) = assimp_mesh->mTextureCoords[0] ? assimp_mesh->mTextureCoords[0][j].y : 0.f;
			}

			// Read faces
			for (uint32_t j = 0; j < num_faces; j++)
			{
				assert(assimp_mesh->mFaces[j].mNumIndices == 3 && "Only support triangle mesh for now!");
				for (uint32_t k = 0; k < 3; k++)
				{
					scene.meshes[i].F(j, k) = assimp_mesh->mFaces[j].mIndices[k];
				}
			}

			// Read base color
			{
				const aiMaterial *assimp_material = s_scene->mMaterials[assimp_mesh->mMaterialIndex];

				aiString texture_path;
				assimp_material->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &texture_path);
				if (texture_map.find(texture_path.C_Str()) != texture_map.end())
				{
					scene.meshes[i].texture = texture_map.at(texture_path.C_Str());
				}
				else
				{
					auto [assimp_texture, texture_id] = s_scene->GetEmbeddedTextureAndIndex(texture_path.C_Str());
					if (texture_id != 0 || texture_path.length > 0)
					{
						int32_t  width = 0, height = 0, channel = 0;
						stbi_uc *raw_data = nullptr;
						if (texture_id < 0)
						{
							raw_data = stbi_load(texture_path.C_Str(), &width, &height, &channel, 3);
						}
						else
						{
							raw_data = stbi_load_from_memory(
							    reinterpret_cast<stbi_uc *>(assimp_texture->pcData),
							    assimp_texture->mWidth * std::max(1u, assimp_texture->mHeight) * 4,
							    &width, &height, &channel, 3);
						}

						Texture texture;
						texture.R.resize(width, height);
						texture.G.resize(width, height);
						texture.B.resize(width, height);

						for (int32_t x = 0; x < width; ++x)
						{
							for (int32_t y = 0; y < height; ++y)
							{
								texture.R(x, y) = raw_data[(y * width + x) * 3 + 0];
								texture.G(x, y) = raw_data[(y * width + x) * 3 + 1];
								texture.B(x, y) = raw_data[(y * width + x) * 3 + 2];
							}
						}

						scene.textures.emplace_back(std::move(texture));
						scene.meshes[i].texture = static_cast<int32_t>(scene.textures.size() - 1);
						stbi_image_free(raw_data);
					}
				}
			}
		}

		// Handle hierarchy
		scene.instances.clear();
		Eigen::Matrix4d transform = Eigen::Matrix4d::Identity();
		aiNode         *root      = scene.assimp_scene->mRootNode;

		std::function<void(aiNode *)> traverse_node = [&](aiNode *node) {
			scene.num_instance++;
			transform *= convert_matrix(node->mTransformation);
			Instance instance;
			instance.transform = transform;
			for (uint32_t i = 0; i < node->mNumMeshes; i++)
			{
				instance.submeshes.push_back(node->mMeshes[i]);
				scene.num_total_mesh++;
			}
			scene.instances.emplace_back(std::move(instance));
			for (uint32_t i = 0; i < node->mNumChildren; i++)
			{
				traverse_node(node->mChildren[i]);
			}
		};

		traverse_node(root);
	}

	return scene;
}

void AssimpHelper::Save(const char *filename, const Scene &scene, const char *flags)
{
	for (uint32_t i = 0; i < scene.assimp_scene->mNumMeshes; i++)
	{
		aiMesh *assimp_mesh = scene.assimp_scene->mMeshes[i];

		const uint32_t num_vertices  = static_cast<uint32_t>(scene.meshes[i].V.rows());
		const uint32_t num_faces     = static_cast<uint32_t>(scene.meshes[i].F.rows());
		const uint32_t num_normals   = static_cast<uint32_t>(scene.meshes[i].N.rows());
		const uint32_t num_texcoords = static_cast<uint32_t>(scene.meshes[i].UV.rows());

		if (assimp_mesh->mNumVertices != num_vertices)
		{
			// Recreate vertices
			{
				assimp_mesh->mNumVertices = num_vertices;
				if (assimp_mesh->mVertices)
				{
					delete[] assimp_mesh->mVertices;
					assimp_mesh->mVertices = nullptr;
				}
				assimp_mesh->mVertices = new aiVector3D[num_vertices];
			}
			// Recreate normals
			{
				if (assimp_mesh->mNormals)
				{
					delete[] assimp_mesh->mNormals;
					assimp_mesh->mNormals = nullptr;
				}
				if (num_normals > 0)
				{
					assimp_mesh->mNormals = new aiVector3D[num_normals];
				}
			}
			// Recreate texcoords 0
			{
				if (assimp_mesh->mTextureCoords[0])
				{
					delete[] assimp_mesh->mTextureCoords[0];
					assimp_mesh->mTextureCoords[0] = nullptr;
				}
				if (num_texcoords > 0)
				{
					assimp_mesh->mTextureCoords[0] = new aiVector3D[num_texcoords];
				}
			}
		}

		if (assimp_mesh->mNumFaces != num_faces)
		{
			assimp_mesh->mNumFaces = num_faces;
			if (assimp_mesh->mNumFaces)
			{
				delete[] assimp_mesh->mFaces;
				assimp_mesh->mFaces = nullptr;
			}
			assimp_mesh->mFaces    = new aiFace[num_faces];
			assimp_mesh->mNumFaces = num_faces;
			for (uint32_t j = 0; j < num_faces; j++)
			{
				assimp_mesh->mFaces[j].mNumIndices = 3;
				assimp_mesh->mFaces[j].mIndices    = new uint32_t[3];
			}
		}

		for (uint32_t j = 0; j < num_vertices; j++)
		{
			assimp_mesh->mVertices[j].x = static_cast<float>(scene.meshes[i].V(j, 0));
			assimp_mesh->mVertices[j].y = static_cast<float>(scene.meshes[i].V(j, 1));
			assimp_mesh->mVertices[j].z = static_cast<float>(scene.meshes[i].V(j, 2));

			if (j < num_normals)
			{
				assimp_mesh->mNormals[j].x = static_cast<float>(scene.meshes[i].N(j, 0));
				assimp_mesh->mNormals[j].y = static_cast<float>(scene.meshes[i].N(j, 1));
				assimp_mesh->mNormals[j].z = static_cast<float>(scene.meshes[i].N(j, 2));
			}

			if (j < num_texcoords)
			{
				assimp_mesh->mTextureCoords[0][j].x = static_cast<float>(scene.meshes[i].UV(j, 0));
				assimp_mesh->mTextureCoords[0][j].y = 1.f - static_cast<float>(scene.meshes[i].UV(j, 1));
			}
		}

		for (uint32_t j = 0; j < num_faces; j++)
		{
			// TODO: Polygon mesh?
			assert(assimp_mesh->mFaces[j].mNumIndices == 3);
			for (uint32_t k = 0; k < 3; k++)
			{
				assimp_mesh->mFaces[j].mIndices[k] = scene.meshes[i].F(j, k);
			}
		}
	}
	Assimp::Exporter exporter;
	exporter.Export(scene.assimp_scene, flags, filename);
}
