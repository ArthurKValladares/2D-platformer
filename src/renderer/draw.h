#pragma once

#include <vulkan/vulkan.h>

// TODO: very incomplete, just the bare minimum for testing
struct DrawCommand {
    uint64_t material_idx;
    std::pair<uint32_t, uint32_t> pipeline_id;
    uint32_t index_count;
    uint32_t first_index;
};