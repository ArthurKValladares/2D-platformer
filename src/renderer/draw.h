#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "resource_ids.h"

struct PushConstantData {
    VkShaderStageFlags stage_flags;
    uint32_t offset;
    uint32_t size;
    const void* p_data;
};

// TODO: very incomplete, just the bare minimum for testing
struct DrawCommand {
    TextureID texture_id;
    ShaderID vertex_id;
    ShaderID fragment_id;
    uint32_t index_count;
    uint32_t first_index;
    std::vector<PushConstantData> pcs;
};