#include "terra/renderer/shader.h"
#include "terra/helpers/error.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"
#include "terra/resources/resource_manager.h"

namespace terra {

Shader::~Shader() {
  if (m_module) {
    wgpuShaderModuleRelease(m_module);
    m_module = nullptr;
  }
}

Shader Shader::create_from_wgsl(WebGPUContext& ctx, std::string_view source, std::string_view label) {
  // 1 Prepare WGSL descriptor
  WGPUShaderSourceWGSL wgsl_desc = WGPU_SHADER_SOURCE_WGSL_INIT;
  wgsl_desc.code = to_wgpu_string_view(source);

  WGPUShaderModuleDescriptor desc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
  desc.nextInChain = &wgsl_desc.chain;
  desc.label       = to_wgpu_string_view(label);

  // 2 Error‐scope around compilation
  request_userdata<bool> compile_ok;

  WGPU_PUSH_ERROR_SCOPE(ctx.get_native_device());
  WGPUShaderModule module = wgpuDeviceCreateShaderModule(ctx.get_native_device(), &desc);
  WGPU_POP_ERROR_SCOPE_CAPTURE_BOOL(ctx.get_native_device(), &compile_ok);

  if (!compile_ok.result) {
    TR_CORE_ERROR("Shader “{}” failed to compile", label);
    // You can choose to throw, or continue with a null module:
  }

  // 3 Wrap and return
  return Shader(module, std::string(label));
}


Shader Shader::from_file(WebGPUContext& ctx, const std::string& path, std::string label) {
    std::string source = ResourceManager::read_file_as_string(path);

    Shader shader = create_from_wgsl(ctx, source, label);
    shader.m_source = std::move(source);
    return shader;
}

Shader::Shader(Shader&& other) noexcept
    : m_module(other.m_module),
      m_source(std::move(other.m_source)),
      label(std::move(other.label)),
      vertex_entry(std::move(other.vertex_entry)),
      fragment_entry(std::move(other.fragment_entry)) {
    other.m_module = nullptr;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_module) {
            wgpuShaderModuleRelease(m_module);
        }
        m_module = other.m_module;
        m_source = std::move(other.m_source);
        label = std::move(other.label);
        vertex_entry = std::move(other.vertex_entry);
        fragment_entry = std::move(other.fragment_entry);
        other.m_module = nullptr;
    }
    return *this;
}

} // namespace terra
