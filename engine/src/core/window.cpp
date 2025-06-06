#include "terrapch.h"
#include "terra/core/window.h"

#ifdef TR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsWindow.h"
#endif
#ifdef TR_PLATFORM_LINUX
    #include "Platform/Linux/LinuxWindow.h"
#endif
#ifdef TR_PLATFORM_MACOS
    #include "terra/platform/macos/window.h"
#endif


namespace terra {

scope<Window> Window::create(const WindowProps& props)
{
    #ifdef TR_PLATFORM_WINDOWS
        return create_scope<WindowsWindow>(props);
    #elif defined(TR_PLATFORM_LINUX)
        return create_scope<LinuxWindow>(props);
    #elif defined(TR_PLATFORM_MACOS)
        return create_scope<MacOSWindow>(props);
    #else
        TR_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
    #endif
}
}
