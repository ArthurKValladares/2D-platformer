#pragma once

#include <vulkan/vulkan.h>

// TODO: very incomplete, just the bare minimum for testing
struct DrawCommand {
    VkDescriptorSet descriptor_set;
    uint32_t index_count;
    uint32_t first_index;
};