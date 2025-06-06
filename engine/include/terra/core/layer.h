#pragma once

#include "terra/core/base.h"
#include "terra/events/event.h"
#include "terra/core/timestep.h"


namespace terra {

class Layer {
public:
    Layer(const std::string& name = "Layer") : m_debug_name(name) {}
    virtual ~Layer() {}

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update(Timestep ts) {}
    virtual void on_ui_render() {}
    virtual void on_event(Event& event) {}

    inline const std::string& get_name() const { return m_debug_name; }
protected:
    std::string m_debug_name;
};

}
