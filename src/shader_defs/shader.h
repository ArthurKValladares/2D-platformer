#pragma once

#include <cstdint>
#include <vector>
#include <array>

#include <glm/mat4x4.hpp>

#include "../renderer/draw.h"
#include "../renderer/renderer.h"
#include "../assets.h"

#define GLOBAL_DESCRIPTOR_SET_IDX 0

struct GlobalShaderData {
    glm::mat4 proj_matrix;
};

inline std::array<VkDescriptorSetLayoutBinding, 1> get_global_set_bindings() {
    return {
        VkDescriptorSetLayoutBinding {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
        }
    };
}

inline void update_global_set(Renderer* renderer, BufferID global_buffer_id, DescriptorSetID global_set_id) {
    Buffer& global_buffer = renderer->get_buffer(global_buffer_id);

    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = global_buffer.raw;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(GlobalShaderData);

    std::array<VkWriteDescriptorSet, 1> descriptor_writes{};
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = renderer->get_descriptor_set_at(global_set_id);
    descriptor_writes[0].dstBinding = 0;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    vkUpdateDescriptorSets(renderer->get_device(), static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
}

// TODO: same optimization question as renderable
struct VertexShader{
    virtual ShaderSource source() const = 0;

    virtual int32_t push_descriptor_set_idx() const = 0;
    virtual void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const = 0;
    virtual void append_push_constant_data(std::vector<PushConstantData>& pcs) const = 0;

    virtual uint32_t vertex_num_floats() const = 0;
};

struct FragmentShader{
    virtual ShaderSource source() const = 0;

    virtual int32_t push_descriptor_set_idx() const = 0;
    virtual void append_push_descriptor_sets(std::vector<PushDescriptorSetData>& sets) const = 0;
    virtual void append_push_constant_data(std::vector<PushConstantData>& pcs) const = 0;
};
