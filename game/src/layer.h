#pragma once

#include <terra/terra.h>
#include "terra/renderer/orthographic_camera.h"

class ExampleLayer : public terra::Layer
{
public:
	ExampleLayer();
	virtual ~ExampleLayer() = default;

	virtual void on_attach() override;
	virtual void on_detach() override;

	void on_update(terra::Timestep ts) override;
	virtual void on_ui_render() override;
	void on_event(terra::Event& e) override;
private:
	terra::ref<terra::Shader> m_shader;
	terra::ref<terra::Material> m_material;
	terra::ref<terra::MaterialInstance> m_material_instance;
	terra::ref<terra::Mesh> m_mesh;
	terra::scope<terra::OrthographicCamera> m_camera;
	terra::scope<terra::Pipeline> m_pipeline;

	terra::f32 m_cycle = 10;
};
