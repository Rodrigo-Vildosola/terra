#pragma once

#include "terrapch.h"

namespace terra {

inline WGPUStringView to_wgpu_string_view(std::string_view view) {
    WGPUStringView result;
    result.length = view.length();
    result.data   = view.data(); // Safe: string_view is non-owning
    return result;
}

constexpr WGPUStringView operator"" _wgpu(const char* str, size_t len) {
    return WGPUStringView{ str, len };
}

}
