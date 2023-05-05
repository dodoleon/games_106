#pragma once

#include <Eigen/Dense>
#include <assimp/scene.h>

#include <string>
#include <vector>

struct aiScene;

struct Texture
{
	Eigen::Matrix<uint8_t, -1, -1> R;
	Eigen::Matrix<uint8_t, -1, -1> G;
	Eigen::Matrix<uint8_t, -1, -1> B;
};

struct Mesh
{
	// dims = num_vertices * 3
	// e.g. The position for i'th vertex: (V(i, 0), V(i, 1), V(i, 2))
	Eigen::MatrixXd V;

	// dims = num_vertices * 3
	// e.g. The normal for i'th vertex: (N(i, 0), N(i, 1), N(i, 2))
	//   => (Might need manual renormalization)
	Eigen::MatrixXd N;

	// dims = num_vertices * 2
	// e.g. The texture coordinate for i'th vertex: (UV(i, 0), UV(i, 1))
	Eigen::MatrixXd UV;

	// dims = num_faces * 3
	// e.g. The associated vertex indices for i'th triangle: (F(i, 0), F(i, 1), F(i, 2))
	//   => Indices start with 0
	// Assimp imports the face as CCW (counter clockwise), that is
	//       x2
	//             x0
	//    x1
	Eigen::MatrixXi F;

	int32_t texture = -1;

	// For rendering
	bool is_visible = true;

	// auxiliary annotations
	// Conflicts with the wireframe, recommend to toggle wireframe off while using this
	using EdgeTuple = std::tuple<Eigen::Vector3d, Eigen::Vector3d, Eigen::RowVector3d>;
	std::vector<EdgeTuple> overlayEdges;
};

struct Instance
{
	std::vector<size_t> submeshes;
	Eigen::Matrix4d     transform;

	// For rendering
	bool is_visible = true;
};

struct Scene
{
	std::string filename;

	const aiScene *assimp_scene;

	std::vector<Mesh>     meshes;
	std::vector<Instance> instances;
	std::vector<Texture>  textures;

	size_t num_mesh       = 0;
	size_t num_instance   = 0;
	size_t num_total_mesh = 0;
};

class AssimpHelper
{
  public:
	static Scene Load(const char *filename, uint32_t flags = 0);
	static void  Save(const char *filename, const Scene &scene, const char *flags);
};