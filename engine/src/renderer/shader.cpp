#include "terra/renderer/shader.h"
#include "terra/helpers/error.h"
#include "terra/helpers/string.h"
#include "terra/helpers/user_data.h"

namespace terra {

Shader::Shader(WebGPUContext& ctx, WGPUShaderModule module, std::string label)
  : m_context(ctx)
  , m_module(module)
  , m_label(std::move(label))
{}

Shader::~Shader() {
  if (m_module) {
    wgpuShaderModuleRelease(m_module);
    m_module = nullptr;
  }
}

Shader Shader::create_from_wgsl(WebGPUContext& ctx, std::string_view source, std::string_view label) {
  // 1️⃣ Prepare WGSL descriptor
  WGPUShaderSourceWGSL wgsl_desc = WGPU_SHADER_SOURCE_WGSL_INIT;
  wgsl_desc.code = to_wgpu_string_view(source);

  WGPUShaderModuleDescriptor desc = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
  desc.nextInChain = &wgsl_desc.chain;
  desc.label       = to_wgpu_string_view(label);

  // 2️⃣ Error‐scope around compilation
  request_userdata<bool> compile_ok;

  WGPU_PUSH_ERROR_SCOPE(ctx.get_native_device());
  WGPUShaderModule module = wgpuDeviceCreateShaderModule(ctx.get_native_device(), &desc);
  WGPU_POP_ERROR_SCOPE_CAPTURE_BOOL(ctx.get_native_device(), &compile_ok);

  if (!compile_ok.result) {
    TR_CORE_ERROR("Shader “{}” failed to compile", label);
    // You can choose to throw, or continue with a null module:
  }

  // 3️⃣ Wrap and return
  return Shader(ctx, module, std::string(label));
}

} // namespace terra
