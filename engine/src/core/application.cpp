#include "terra/core/application.h"
#include "terra/core/assert.h"
#include "terra/core/logger.h"
#include "terra/core/timestep.h"

#include "terra/renderer/renderer_api.h"
#include "terra/core/window.h"

namespace terra {

Application* Application::s_instance = nullptr;

Application::Application(const std::string& name, CommandLineArgs args)
    : m_command_line_args(args)
{
    TR_CORE_ASSERT(!s_instance, "Application already exists!");
    s_instance = this;

    TR_CORE_INFO("Creating window");
    m_window = Window::create(WindowProps(name));
	m_window->set_event_cb(TR_BIND_EVENT_FN(Application::on_event));

    m_context = WebGPUContext::create();
    m_context->init(m_window.get());

    RendererAPI::init(m_context.get());

    #if !defined(TR_RELEASE)
        TR_CORE_INFO("Creating ImGui layer");
        m_ui_layer = new UILayer();
        push_overlay(m_ui_layer);
    #endif
}

Application::~Application() {
    TR_CORE_INFO("Shutting down Terra Engine...");
    RendererAPI::shutdown();
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
    Timer::init();

    // render loop
    // -----------
    while (m_running) {
        f32 time = Timer::elapsed();
        Timestep timestep = time - m_last_frame_time;
        m_last_frame_time = time;

        RendererAPI::begin_frame();

        if (!m_minimized) {
            for (Layer* layer : m_layer_stack)
                layer->on_update(timestep);
        }

        #if !defined(TR_RELEASE)
            RendererAPI::begin_ui_pass();
            m_ui_layer->begin();
            for (Layer* layer : m_layer_stack)
                layer->on_ui_render();
            m_ui_layer->end();
            RendererAPI::end_ui_pass();
		#endif

        RendererAPI::end_frame();

        m_window->on_update();

        RendererAPI::get_stats_mutable().frame_time_ms = timestep.get_milliseconds();
        RendererAPI::get_stats_mutable().fps = 1.0f / timestep.get_seconds();

        // break;
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

    m_context->configure_surface(m_context->get_preferred_format());

    return false;
}

bool Application::on_window_close(WindowCloseEvent& e) {
    m_running = false;
    return true;
}


// bool Application::initialize_buffers() {
//     std::vector<f32> vertex_data = {
//         // Triangle 1
//         -0.45f, 0.5f,
//         0.45f, 0.5f,
//         0.0f, -0.5f,

//         // Triangle 2
//         0.47f, 0.47f,
//         0.25f, 0.0f,
//         0.69f, 0.0f
//     };

//     u32 vertex_count = (u32)(vertex_data.size() / 2);
    
//     WGPUBuffer vertex_buffer = Buffer::create(
//         m_context->get_native_device(),
//         m_context->get_queue()->get_native_queue(),
//         vertex_data.data(),
//         vertex_data.size() * sizeof(f32),
//         WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
//         "Vertex Buffer"
//     );

// }


}


