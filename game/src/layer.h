#pragma once

#include <terra/terra.h>
#include "terra/renderer/orthographic_camera.h"
#include "terra/renderer/perspective_camera.h"

class ExampleLayer : public terra::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;

	void on_update(terra::Timestep ts) override;
	void on_physics_update(terra::Timestep fixed_ts) override;
	virtual void on_ui_render() override;
	void on_event(terra::Event& e) override;

private:
	terra::ref<terra::Shader> m_shader;
	terra::ref<terra::Material> m_material;
	terra::ref<terra::MaterialInstance> m_material_instance;
	terra::ref<terra::Mesh> m_mesh;
	terra::scope<terra::PerspectiveCamera> m_camera;
	terra::scope<terra::Pipeline> m_pipeline;

	terra::f32 m_cycle = 10;
	float m_displayed_fps = 0.0f;
	float m_displayed_frame_time = 0.0f;
	float m_ui_stats_timer = 0.0f;

};
