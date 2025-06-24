#pragma once

#include "terrapch.h"
#include "terra/renderer/material_instance.h"
#include "terra/renderer/shader.h"
#include <memory>

namespace terra {

class WebGPUContext;

// Material template that defines the structure and default values
class Material {
public:
    Material(WebGPUContext& context, const std::string& name);
    ~Material() = default;

    // Create a new instance of this material
    ref<MaterialInstance> create_instance(Pipeline* pipeline);
    
    // Material properties
    void set_name(const std::string& name) { m_name = name; }
    const std::string& get_name() const { return m_name; }
    
    // Shader management
    void set_shader(ref<Shader> shader) { m_shader = shader; }
    ref<Shader> get_shader() const { return m_shader; }
    
    // Parameter definitions
    void define_parameter(
        const std::string& name, 
        u32 binding, 
        MaterialParamType type, 
        WGPUShaderStage visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment
    );
    
    // Get parameter information
    bool has_parameter(const std::string& name) const;
    u32 get_parameter_binding(const std::string& name) const;
    MaterialParamType get_parameter_type(const std::string& name) const;
    
    // Default values
    void set_default_value(const std::string& name, const void* data, size_t size);
    void set_default_float(const std::string& name, f32 value);
    void set_default_float3(const std::string& name, const f32* values);
    void set_default_matrix4x4(const std::string& name, const f32* matrix);

private:
    WebGPUContext& m_context;
    std::string m_name;
    ref<Shader> m_shader;
    
    struct ParameterDefinition {
        u32 binding;
        MaterialParamType type;
        WGPUShaderStage visibility;
        std::vector<u8> default_value;
    };
    
    std::unordered_map<std::string, ParameterDefinition> m_parameters;
};

} // namespace terra 
