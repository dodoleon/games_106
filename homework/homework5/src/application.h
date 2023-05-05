#pragma once

#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiPlugin.h>

#include "assimp_helper.h"

class Application
{
  public:
	Application();

	~Application() = default;

	void Run();

  private:
	// Draw UI
	void DrawUI();

	// Load a new scene
	void LoadScene();

	// Save current scene
	void SaveScene();

	// Display current scene
	void SceneInfo();

	// Reset scene color
	void SceneColor(bool reset = false);

	// Update scene buffer
	void UpdateSceneViewer();

	// Edit scene
	void EditScene();

  private:
	igl::opengl::glfw::Viewer             m_viewer;
	igl::opengl::glfw::imgui::ImGuiPlugin m_plugin;
	igl::opengl::glfw::imgui::ImGuiMenu   m_menu;

	Scene m_raw_scene;
	Scene m_scene;

	enum class ColorConfig
	{
		Texture,
		Mesh,
		Submesh,
		Instance
	};

	enum class ReductionConfig
	{
		Reduction,
		ReductionWithAppearancePresentation,
		ReductionWithUVMap,
	};

	ColorConfig     m_color_config     = ColorConfig::Texture;
	ReductionConfig m_reduction_config = ReductionConfig::Reduction;
	float           m_reduction_ratio  = 0.5f;
};