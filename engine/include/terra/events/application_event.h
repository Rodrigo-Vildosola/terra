#pragma once

#include "terrapch.h"
#include "terra/events/event.h"

namespace terra {

    class WindowResizeEvent : public Event {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : m_width(width), m_height(height) {}

        inline unsigned int get_width() const { return m_width; }
        inline unsigned int get_height() const { return m_height; }

        std::string to_string() const override {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_width << ", " << m_height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    private:
        unsigned int m_width, m_height;
    };

    class WindowCloseEvent : public Event {
    public:
        WindowCloseEvent() {}

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppTickEvent : public Event {
    public:
        AppTickEvent() {}

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public Event {
    public:
        AppUpdateEvent() {}

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent : public Event {
    public:
        AppRenderEvent() {}

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}
