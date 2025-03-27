#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "resource_ids.h"
#include "buffer.h"

struct PushDescriptorSetData {
    uint32_t set;
    uint32_t binding;
    VkDescriptorType ty;
    // TODO: Might need some more data on exactly how to use the texture/buffer in the future
    TextureID texture_id;
    BufferID buffer_id;
};

struct PushConstantData {
    VkShaderStageFlags stage_flags;
    uint32_t offset;
    uint32_t size;
    const void* p_data;
};

// TODO: very incomplete, just the bare minimum for testing
struct DrawCommand {
    ShaderID vertex_id;
    ShaderID fragment_id;
    uint32_t index_count;
    uint32_t first_index;
    // TODO: This should be std::array<std::vector<DescriptorSetData>, MAX_DESCRITOR_SETS>
    uint32_t push_set_idx;
    std::vector<PushDescriptorSetData> push_set_data;
    std::vector<PushConstantData> pcs;
};