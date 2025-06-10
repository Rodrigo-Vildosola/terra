#include "terra/core/application.h"
#include "terra/core/assert.h"
#include "terra/core/timestep.h"

#include "terra/renderer/renderer.h"
#include "terra/core/window.h"

namespace terra {

Application* Application::s_instance = nullptr;

scope<GraphicsContext> Application::s_context;


Application::Application(const std::string& name, CommandLineArgs args)
    : m_command_line_args(args)
{
    TR_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    TR_CORE_INFO("Creating window");
    m_window = Window::create(WindowProps(name));
	m_window->set_event_cb(TR_BIND_EVENT_FN(Application::on_event));

    s_context = GraphicsContext::create(m_window->get_native_window());
    s_context->init();


    TR_CORE_TRACE("Renderer initialized");

    #if !defined(TR_RELEASE)
        TR_CORE_INFO("Creating ImGui layer");
        m_ui_layer = new UILayer();
        push_overlay(m_ui_layer);
    #endif
}

Application::~Application() {
    TR_CORE_INFO("Shutting down Terra Engine...");
    // Renderer::shutdown();
    m_window.reset();
}

void Application::push_layer(Layer* layer) {
    m_layer_stack.push_layer(layer);
    layer->on_attach();
}

void Application::push_overlay(Layer* layer) {

    m_layer_stack.push_overlay(layer);
    layer->on_attach();
}

void Application::on_event(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<WindowCloseEvent>(TR_BIND_EVENT_FN(Application::on_window_close));
    dispatcher.dispatch<WindowResizeEvent>(TR_BIND_EVENT_FN(Application::on_window_resize));

    for (auto it = m_layer_stack.rbegin(); it != m_layer_stack.rend(); ++it)
    {
        if (e.handled) 
            break;
        (*it)->on_event(e);
    }
}


void Application::run() {
    Renderer::init(m_window->get_native_window());
    Timer::init();

    // render loop
    // -----------
    while (m_running) {

        float time = Timer::elapsed();
        Timestep timestep = time - m_last_frame_time;
        m_last_frame_time = time;

        if (!m_minimized) {
            for (Layer* layer : m_layer_stack)
                layer->on_update(timestep);
        }

        // Renderer::begin_frame();


        #if defined(TR_RELEASE)
            m_ui_layer->begin();
            for (Layer* layer : m_layer_stack)
                layer->on_ui_render();
            m_ui_layer->end();
		#endif


        // Renderer::draw();
        // Renderer::end_frame();


        m_window->on_update();
    }

}

void Application::close() {
    m_running = false;
}



bool Application::on_window_resize(WindowResizeEvent& e) {
    if (e.get_width() == 0 || e.get_height() == 0)
    {
        m_minimized = true;
        return false;
    }

    m_minimized = false;
    // Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

bool Application::on_window_close(WindowCloseEvent& e) {
    m_running = false;
    return true;
}
}
