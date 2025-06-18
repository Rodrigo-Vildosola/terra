#pragma once

#include "terra/core/layer.h"
#include "terra/events/application_event.h"
#include "terra/events/key_event.h"
#include "terra/events/mouse_event.h"


namespace terra {

class UILayer : public Layer
{
public:
    UILayer();
    ~UILayer();

    virtual void on_attach() override;
    virtual void on_detach() override;
    virtual void on_event(Event& e) override;

    void begin();
    void end();
    void block_events(bool block) { m_block_events = block; }
    void set_dark_theme_colors();
private:
    bool m_block_events = true;
    float m_time = 0.0f;
};

}
