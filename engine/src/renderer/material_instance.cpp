#include "terra/renderer/material_instance.h"
#include "terra/core/context/command_queue.h"
#include "terra/renderer/buffer.h"
#include "terra/core/assert.h"
#include "terra/core/logger.h"

namespace terra {

MaterialInstance::MaterialInstance(WebGPUContext& context, Pipeline* pipeline)
    : m_context(context), m_pipeline(pipeline) {
    create_uniform_buffers();
    create_bind_group();
}

MaterialInstance::~MaterialInstance() {
    if (m_bind_group) {
        wgpuBindGroupRelease(m_bind_group);
    }
}

void MaterialInstance::create_uniform_buffers() {
    const auto& spec = m_pipeline->get_specification();

    for (const auto& uniform : spec.uniforms) {
        UniformBuffer ub = Buffer::create_uniform_buffer(
            m_context,
            nullptr, // no initial data
            uniform.size,
            uniform.binding,
            "Material Uniform"
        );
        m_uniforms.push_back(ub);
        
        // Initialize parameter with default type (Float for now)
        MaterialParam param(
            MaterialParamType::Float, 
            uniform.binding, 
            uniform.size, 
            uniform.visibility
        );
        m_parameters[uniform.binding] = param;
    }
}

void MaterialInstance::create_bind_group() {
    std::vector<WGPUBindGroupEntry> entries;

    for (size_t i = 0; i < m_uniforms.size(); ++i) {
        const auto& ub = m_uniforms[i];

        WGPUBindGroupEntry entry = WGPU_BIND_GROUP_ENTRY_INIT;
        entry.binding = ub.binding;
        entry.buffer  = ub.buffer;
        entry.offset  = 0;
        entry.size    = ub.size;
        entries.push_back(entry);
    }

    WGPUBindGroupDescriptor desc = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
    desc.layout = m_pipeline->get_bind_group_layout();
    desc.entryCount = static_cast<u32>(entries.size());
    desc.entries = entries.data();

    m_bind_group = wgpuDeviceCreateBindGroup(m_context.get_native_device(), &desc);
}

void MaterialInstance::set_uniform_data(u32 binding_index, const void* data, u64 size) {
    TR_CORE_ASSERT(binding_index < m_uniforms.size(), "Invalid uniform binding index");

    auto it = m_parameters.find(binding_index);
    if (it != m_parameters.end()) {
        it->second.data.resize(size);
        std::memcpy(it->second.data.data(), data, size);
        update_uniform_buffer(binding_index);
    }
}

// Type-safe uniform setters
void MaterialInstance::set_float(u32 binding, f32 value) {
    set_uniform_data(binding, &value, sizeof(f32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Float;
    }
}

void MaterialInstance::set_float2(u32 binding, const f32* values) {
    set_uniform_data(binding, values, 2 * sizeof(f32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Float2;
    }
}

void MaterialInstance::set_float3(u32 binding, const f32* values) {
    set_uniform_data(binding, values, 3 * sizeof(f32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Float3;
    }
}

void MaterialInstance::set_float4(u32 binding, const f32* values) {
    set_uniform_data(binding, values, 4 * sizeof(f32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Float4;
    }
}

void MaterialInstance::set_int(u32 binding, i32 value) {
    set_uniform_data(binding, &value, sizeof(i32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Int;
    }
}

void MaterialInstance::set_int2(u32 binding, const i32* values) {
    set_uniform_data(binding, values, 2 * sizeof(i32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Int2;
    }
}

void MaterialInstance::set_int3(u32 binding, const i32* values) {
    set_uniform_data(binding, values, 3 * sizeof(i32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Int3;
    }
}

void MaterialInstance::set_int4(u32 binding, const i32* values) {
    set_uniform_data(binding, values, 4 * sizeof(i32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Int4;
    }
}

void MaterialInstance::set_matrix4x4(u32 binding, const f32* matrix) {
    set_uniform_data(binding, matrix, 16 * sizeof(f32));
    auto it = m_parameters.find(binding);
    if (it != m_parameters.end()) {
        it->second.type = MaterialParamType::Matrix4x4;
    }
}

// Named parameter setters
void MaterialInstance::set_parameter(const std::string& name, const void* data, u64 size) {
    auto it = m_parameter_bindings.find(name);
    if (it != m_parameter_bindings.end()) {
        set_uniform_data(it->second, data, size);
    }
}

void MaterialInstance::set_parameter_float(const std::string& name, f32 value) {
    auto it = m_parameter_bindings.find(name);
    if (it != m_parameter_bindings.end()) {
        set_float(it->second, value);
    }
}

void MaterialInstance::set_parameter_float3(const std::string& name, const f32* values) {
    auto it = m_parameter_bindings.find(name);
    if (it != m_parameter_bindings.end()) {
        set_float3(it->second, values);
    }
}

void MaterialInstance::set_parameter_matrix4x4(const std::string& name, const f32* matrix) {
    auto it = m_parameter_bindings.find(name);
    if (it != m_parameter_bindings.end()) {
        set_matrix4x4(it->second, matrix);
    }
}

void MaterialInstance::set_parameter_binding(const std::string& name, u32 binding) {
    m_parameter_bindings[name] = binding;
}

void MaterialInstance::bind(WGPURenderPassEncoder pass_encoder) {
    wgpuRenderPassEncoderSetBindGroup(pass_encoder, 0, m_bind_group, 0, nullptr);
}

WGPUBindGroup MaterialInstance::get_bind_group(u32 index) const {
    TR_CORE_ASSERT(index == 0, "Only one bind group currently supported");
    return m_bind_group;
}

void MaterialInstance::update_uniforms() {
    for (const auto& [binding, param] : m_parameters) {
        update_uniform_buffer(binding);
    }
}

void MaterialInstance::update_uniform_buffer(u32 binding_index) {
    TR_CORE_ASSERT(binding_index < m_uniforms.size(), "Invalid uniform binding index");

    auto param_it = m_parameters.find(binding_index);
    if (param_it == m_parameters.end()) return;

    const auto& param = param_it->second;
    const auto& ub = m_uniforms[binding_index];

    if (!param.data.empty()) {
        wgpuQueueWriteBuffer(
            m_context.get_queue()->get_native_queue(),
            ub.buffer,
            0,
            param.data.data(),
            param.data.size()
        );
    }
}

u64 MaterialInstance::get_parameter_size(MaterialParamType type) const {
    switch (type) {
        case MaterialParamType::Float: return sizeof(f32);
        case MaterialParamType::Float2: return 2 * sizeof(f32);
        case MaterialParamType::Float3: return 3 * sizeof(f32);
        case MaterialParamType::Float4: return 4 * sizeof(f32);
        case MaterialParamType::Int: return sizeof(i32);
        case MaterialParamType::Int2: return 2 * sizeof(i32);
        case MaterialParamType::Int3: return 3 * sizeof(i32);
        case MaterialParamType::Int4: return 4 * sizeof(i32);
        case MaterialParamType::Matrix4x4: return 16 * sizeof(f32);
        default: return 0;
    }
}

}
