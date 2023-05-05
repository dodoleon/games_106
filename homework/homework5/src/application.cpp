#include "application.h"
#include "mesh_reduction.h"

#include <assimp/postprocess.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>

Eigen::RowVector3d GetColor(size_t a)
{
	a = (a + 0x7ed55d16) + (a << 12);
	a = (a ^ 0xc761c23c) ^ (a >> 19);
	a = (a + 0x165667b1) + (a << 5);
	a = (a + 0xd3a2646c) ^ (a << 9);
	a = (a + 0xfd7046c5) + (a << 3);
	a = (a ^ 0xb55a4f09) ^ (a >> 16);
	return Eigen::RowVector3d(float(a & 255), float((a >> 8) & 255), float((a >> 16) & 255)) / 255.0;
}

Application::Application()
{
	m_viewer.plugins.push_back(&m_plugin);
	m_plugin.widgets.push_back(&m_menu);
}

void Application::Run()
{
	m_menu.callback_draw_viewer_window = [&]() {
		DrawUI();
	};
	m_viewer.data_list[0].show_texture = true;
	m_viewer.launch();
}

void Application::DrawUI()
{
	ImGui::Text("GAMES 106 HW 5");
	ImGui::Text("Mesh Simplification");
	SceneColor();
	LoadScene();
	SaveScene();
	SceneInfo();
	EditScene();
	glfwSetWindowTitle(m_viewer.window, "GAMES 106 - HW5");
}

void Application::LoadScene()
{
	if (ImGui::CollapsingHeader("Load Scene"))
	{
		static std::map<int32_t, bool> loading_options = {
		    {aiProcess_Triangulate, false},
		    {aiProcess_GenSmoothNormals, false},
		    {aiProcess_JoinIdenticalVertices, false},
		};
		{
			ImGui::Checkbox("Triangulate", &loading_options[aiProcess_Triangulate]);
			ImGui::Checkbox("GenSmoothNormals", &loading_options[aiProcess_GenSmoothNormals]);
			ImGui::Checkbox("JoinIdenticalVertices", &loading_options[aiProcess_JoinIdenticalVertices]);
		}
		if (ImGui::Button("Load"))
		{
			uint32_t flags = 0;
			for (auto &[option, enable] : loading_options)
			{
				if (enable)
				{
					flags |= option;
				}
			}
			std::string filename = igl::file_dialog_open();
			m_raw_scene          = AssimpHelper::Load(filename.c_str(), flags);
			m_scene              = m_raw_scene;
			UpdateSceneViewer();
		}
	}
}

void Application::SaveScene()
{
	if (ImGui::CollapsingHeader("Save Scene"))
	{
		const char *const export_formats[] = {"gltf2", "glb2"};
		static int32_t    export_format    = 0;
		ImGui::Combo("", &export_format, export_formats, 2);
		ImGui::SameLine();
		if (ImGui::Button("Save"))
		{
			std::string filename = igl::file_dialog_save();
			AssimpHelper::Save(filename.c_str(), m_scene, export_formats[export_format]);
		}
	}
}

void Application::SceneInfo()
{
	bool change_visible = false;

	if (ImGui::CollapsingHeader("Scene Info"))
	{
		ImGui::Text("Num Meshes: %d", m_scene.num_mesh);
		ImGui::Text("Num Instances: %d", m_scene.num_instance);

		if (ImGui::TreeNode("Meshes"))
		{
			uint32_t count = 0;
			for (auto &mesh : m_scene.meshes)
			{
				ImGui::PushID(&mesh);
				if (ImGui::TreeNode((std::string("Meshes #") + std::to_string(count++)).c_str()))
				{
					ImGui::Text("Num Vertices: %d", mesh.V.rows());
					ImGui::Text("Num Triangles: %d", mesh.F.rows());
					if (ImGui::Checkbox("Visible", &mesh.is_visible))
					{
						change_visible = true;
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Instances"))
		{
			uint32_t count = 0;
			for (auto &instance : m_scene.instances)
			{
				ImGui::PushID(&instance);
				if (ImGui::TreeNode((std::string("Instances #") + std::to_string(count++)).c_str()))
				{
					ImGui::Text("Num Submeshes: %ld", instance.submeshes.size());
					if (ImGui::Checkbox("Visible", &instance.is_visible))
					{
						change_visible = true;
					}
					ImGui::TreePop();
				}
				ImGui::PopID();
			}
			ImGui::TreePop();
		}

		if (change_visible)
		{
			size_t id = 0;
			for (auto &instance : m_scene.instances)
			{
				for (auto &mesh_id : instance.submeshes)
				{
					const auto &mesh = m_scene.meshes[mesh_id];
					m_viewer.data_list[id].set_visible(instance.is_visible && mesh.is_visible);
					++id;
				}
			}
		}
	}
}

void Application::SceneColor(bool reset)
{
	const char *const color_config[] = {"Texture", "Mesh", "Submesh", "Instance"};
	if (reset || ImGui::Combo("Color Config", reinterpret_cast<int32_t *>(&m_color_config), color_config, 4))
	{
		size_t instance_id = 0;
		size_t submesh_id  = 0;
		for (auto &instance : m_scene.instances)
		{
			for (auto &mesh : instance.submeshes)
			{
				switch (m_color_config)
				{
					case ColorConfig::Texture:
						m_viewer.data_list[submesh_id].set_colors(Eigen::RowVector3d(1.0, 1.0, 1.0));
						m_viewer.data_list[submesh_id].show_texture = true;
						break;
					case ColorConfig::Mesh:
						m_viewer.data_list[submesh_id].set_colors(GetColor(mesh));
						m_viewer.data_list[submesh_id].show_texture = false;
						break;
					case ColorConfig::Submesh:
						m_viewer.data_list[submesh_id].set_colors(GetColor(submesh_id));
						m_viewer.data_list[submesh_id].show_texture = false;
						break;
					case ColorConfig::Instance:
						m_viewer.data_list[submesh_id].set_colors(GetColor(instance_id));
						m_viewer.data_list[submesh_id].show_texture = false;
						break;
					default:
						break;
				}
				++submesh_id;
			}
			++instance_id;
		}
	}
}

void Application::UpdateSceneViewer()
{
	m_viewer.data_list.clear();
	m_viewer.data_list.resize(m_scene.num_total_mesh);
	// Set scene
	size_t id = 0;
	for (auto &instance : m_scene.instances)
	{
		for (auto &mesh_id : instance.submeshes)
		{
			const auto &mesh = m_scene.meshes[mesh_id];
			m_viewer.data_list[id].set_mesh((mesh.V.rowwise().homogeneous() * instance.transform).rowwise().hnormalized(), mesh.F);
			m_viewer.data_list[id].set_uv(mesh.UV);
			m_viewer.data_list[id].set_normals((mesh.N.rowwise().homogeneous() * instance.transform).rowwise().hnormalized());
			if (mesh.texture >= 0)
			{
				const auto &texture = m_scene.textures[mesh.texture];
				m_viewer.data_list[id].set_texture(texture.R, texture.G, texture.B);
			}

			if (mesh.overlayEdges.size() > 0)
			{
				Eigen::MatrixXd P1, P2, C;
				P1.resize(mesh.overlayEdges.size(), 3);
				P2.resizeLike(P1);
				C.resizeLike(P1);

				int idx = 0;
				for (auto& overlayEdge : mesh.overlayEdges)
				{
					P1.row(idx) = std::get<0>(overlayEdge);
					P2.row(idx) = std::get<1>(overlayEdge);
					C.row(idx)  = std::get<2>(overlayEdge);
					idx++;
				}
				m_viewer.data_list[id].add_edges(P1, P2, C);
			}

			m_viewer.data_list[id].set_visible(instance.is_visible && mesh.is_visible);
			m_viewer.data_list[id].show_texture = true;
			++id;
		}
	}
	SceneColor(true);
}

void Application::EditScene()
{
	if (ImGui::CollapsingHeader("Mesh Reduction"))
	{
		const char *const reduction_configs[] = {
		    "Reduction",
		    "ReductionWithAppearancePresentation",
		    "ReductionWithUVMap",
		};
		ImGui::Combo("Config", reinterpret_cast<int32_t *>(&m_reduction_config), reduction_configs, 3);
		ImGui::DragFloat("Ratio", &m_reduction_ratio, 0.01f, 0.f, 1.f, "%.2f");
		if (ImGui::Button("Reduction"))
		{
			for (uint32_t i = 0; i < m_scene.meshes.size(); i++)
			{
				switch (m_reduction_config)
				{
					case ReductionConfig::Reduction:
						m_scene.meshes[i] = MeshReduction::Reduction(m_raw_scene.meshes[i], m_reduction_ratio);
						break;
					case ReductionConfig::ReductionWithAppearancePresentation:
						m_scene.meshes[i] = MeshReduction::ReductionWithAppearancePresentation(m_raw_scene.meshes[i], m_reduction_ratio);
						break;
					case ReductionConfig::ReductionWithUVMap:
						m_scene.meshes[i] = MeshReduction::ReductionWithUVMap(m_raw_scene.meshes[i], m_reduction_ratio);
						break;
					default:
						break;
				}
			}
			UpdateSceneViewer();
		}
	}
}
