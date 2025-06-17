#pragma once

#include "terra/core/context/context.h"
#include <string>

namespace terra {

/// Encapsulates a single shader module (WGSL or SPIR-V).
class Shader {
public:
  /// Read WGSL from `source` (later you can add loadFromFile)
  /// `label` is used for debug markers.
  static Shader create_from_wgsl(WebGPUContext& ctx, std::string_view source, std::string_view label);

  ~Shader();

  /// The raw WGPUShaderModule; for pipeline creation only.
  WGPUShaderModule module() const { return m_module; }

  /// Debug label
  const std::string& label() const { return m_label; }

private:
  Shader(WebGPUContext& ctx, WGPUShaderModule module, std::string label);

  WebGPUContext&   m_context;
  WGPUShaderModule m_module;
  std::string      m_label;
};

} // namespace terra
