#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "resource_ids.h"
#include "buffer.h"

struct PushDescriptorSetData {
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

struct DrawCommand {
    ShaderID vertex_id;
    ShaderID fragment_id;
    bool alpha_blending;
    uint32_t index_count;
    uint32_t first_index;
    // regular sets
    std::vector<DescriptorSetID> set_ids;
    // push sets
    uint32_t push_set_idx;
    std::vector<PushDescriptorSetData> push_set_data;
    // push constants
    std::vector<PushConstantData> pcs;
};