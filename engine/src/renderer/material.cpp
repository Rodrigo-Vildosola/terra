#include "terrapch.h"
#include "terra/core/assert.h"
#include "terra/renderer/material.h"
#include "terra/renderer/pipeline.h"

namespace terra {

Material::Material(WebGPUContext& context, const std::string& name)
    : m_context(context), m_name(name) {
}

ref<MaterialInstance> Material::create_instance(Pipeline* pipeline) {
    const auto& pipeline_spec = pipeline->get_specification();
    
    // Optionally check compatibility
    for (auto& [name, param] : m_parameters) {
        bool found = std::any_of(
            pipeline_spec.uniforms.begin(), 
            pipeline_spec.uniforms.end(), 
            [&](const UniformBufferSpec& u) { return u.binding == param.binding && u.visibility == param.visibility; }
        );
        TR_CORE_ASSERT(found, "Pipeline is not compatible with material parameter");
    }

    auto instance = create_ref<MaterialInstance>(m_context, pipeline);
    instance->set_name(m_name + "_Instance");
    
    // Set up parameter bindings
    for (const auto& [name, param_def] : m_parameters) {
        instance->set_parameter_binding(name, param_def.binding);
    }

    // Apply default values to the instance
    for (const auto& [name, param_def] : m_parameters) {
        if (!param_def.default_value.empty()) {
            instance->set_uniform_data(
                param_def.binding, 
                param_def.default_value.data(), 
                param_def.default_value.size()
            );
        }
    }

    return instance;
}

void Material::define_parameter(const std::string& name, u32 binding, MaterialParamType type, WGPUShaderStage visibility) {
    ParameterDefinition def;
    def.binding = binding;
    def.type = type;
    def.visibility = visibility;
    m_parameters[name] = def;
}

bool Material::has_parameter(const std::string& name) const {
    return m_parameters.find(name) != m_parameters.end();
}

u32 Material::get_parameter_binding(const std::string& name) const {
    auto it = m_parameters.find(name);
    TR_CORE_ASSERT(it != m_parameters.end(), "Parameter not found");
    return it->second.binding;
}

MaterialParamType Material::get_parameter_type(const std::string& name) const {
    auto it = m_parameters.find(name);
    TR_CORE_ASSERT(it != m_parameters.end(), "Parameter not found");
    return it->second.type;
}

void Material::set_default_value(const std::string& name, const void* data, size_t size) {
    auto it = m_parameters.find(name);
    TR_CORE_ASSERT(it != m_parameters.end(), "Parameter not found");
    
    it->second.default_value.resize(size);
    std::memcpy(it->second.default_value.data(), data, size);
}

void Material::set_default_float(const std::string& name, f32 value) {
    set_default_value(name, &value, sizeof(f32));
}

void Material::set_default_float3(const std::string& name, const f32* values) {
    set_default_value(name, values, 3 * sizeof(f32));
}

void Material::set_default_matrix4x4(const std::string& name, const f32* matrix) {
    set_default_value(name, matrix, 16 * sizeof(f32));
}

} // namespace terra 
