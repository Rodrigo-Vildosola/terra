#pragma once

#include <terra/terra.h>

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

};
