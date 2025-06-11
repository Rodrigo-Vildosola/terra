#include "terrapch.h"
#include "terra/renderer/renderer.h"
#include <GLFW/glfw3.h>

namespace terra {


// Allocate static context
void Renderer::init(void* native_window) {
    TR_CORE_ASSERT(native_window, "Renderer init requires a native window pointer!");

    WGPUDevice device = Application::get().get_device();
    m_queue = RenderQueue::create();
    m_queue->init(device);


}

}
