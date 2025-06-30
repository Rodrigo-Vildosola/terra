#pragma once

#include "terrapch.h"
#include "terra/renderer/pipeline.h"
#include "terra/renderer/buffer.h"


namespace terra {

// Forward declarations
class Shader;

// Material parameter types
enum class MaterialParamType {
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Int2,
    Int3,
    Int4,
    Matrix4x4,
    Custom
};

// Material parameter structure
struct MaterialParam {
    MaterialParamType type;
    std::vector<u8> data;
    u32 binding;
    u64 size;
    WGPUShaderStage visibility;
    
    MaterialParam() = default;
    MaterialParam(MaterialParamType t, u32 bind, u64 sz, WGPUShaderStage vis)
        : type(t), binding(bind), size(sz), visibility(vis) {}
};

class MaterialInstance {
public:
    MaterialInstance(WebGPUContext& context, Pipeline* pipeline);
    ~MaterialInstance();

    // Uniform data setters
    void set_uniform_data(u32 binding_index, const void* data, u64 size);


    template<typename T>
    void set_uniform_block(const T& block) {
        set_uniform_data(0, &block, sizeof(T));
    }
    
    // Type-safe uniform setters
    void set_float(u32 binding, f32 value);
    void set_float2(u32 binding, const f32* values);
    void set_float3(u32 binding, const f32* values);
    void set_float4(u32 binding, const f32* values);
    void set_int(u32 binding, i32 value);
    void set_int2(u32 binding, const i32* values);
    void set_int3(u32 binding, const i32* values);
    void set_int4(u32 binding, const i32* values);
    void set_matrix4x4(u32 binding, const f32* matrix);
    
    // Named parameter setters (for shader reflection)
    void set_parameter(const std::string& name, const void* data, u64 size);
    void set_parameter_float(const std::string& name, f32 value);
    void set_parameter_float3(const std::string& name, const f32* values);
    void set_parameter_matrix4x4(const std::string& name, const f32* matrix);
    
    // Parameter binding management
    void set_parameter_binding(const std::string& name, u32 binding);
    
    // Binding
    void bind(WGPURenderPassEncoder pass_encoder);
    WGPUBindGroup get_bind_group(u32 index = 0) const;

    void bind_storage_buffer(u32 group, u32 binding, WGPUBuffer buffer);
    
    // Material properties
    void set_name(const std::string& name) { m_name = name; }
    const std::string& get_name() const noexcept { return m_name; }

    // Pipeline access
    Pipeline* get_pipeline() const noexcept { return m_pipeline; }
    
    // Update all uniforms to GPU
    void update_uniforms();

    constexpr u64 get_parameter_size(MaterialParamType type) const;

private:
    WebGPUContext& m_context;
    Pipeline* m_pipeline = nullptr;

    std::string m_name;

    std::vector<UniformBuffer> m_uniforms;
    std::unordered_map<u32, MaterialParam> m_parameters;
    std::unordered_map<std::string, u32> m_parameter_bindings;

    WGPUBindGroup m_bind_group = nullptr;

    std::unordered_map<u32, WGPUBindGroup> m_storage_bind_groups;

    void create_uniform_buffers();
    void create_bind_group();
    void update_uniform_buffer(u32 binding_index);
    
    template<typename T>
    void set_typed(u32 binding, const T* value, MaterialParamType type);
};

}
