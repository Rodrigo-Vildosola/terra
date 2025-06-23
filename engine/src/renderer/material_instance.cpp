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

void MaterialInstance::set_uniform_data(u32 binding_index, const void* data, size_t size) {
    TR_CORE_ASSERT(binding_index < m_uniforms.size(), "Invalid uniform binding index");

    const auto& ub = m_uniforms[binding_index];

    wgpuQueueWriteBuffer(
        m_context.get_queue()->get_native_queue(),
        ub.buffer,
        0,
        data,
        size
    );
}

void MaterialInstance::bind(WGPURenderPassEncoder pass_encoder) {
    wgpuRenderPassEncoderSetBindGroup(pass_encoder, 0, m_bind_group, 0, nullptr);
}

WGPUBindGroup MaterialInstance::get_bind_group(u32 index) const {
    TR_CORE_ASSERT(index == 0, "Only one bind group currently supported");
    return m_bind_group;
}

}
