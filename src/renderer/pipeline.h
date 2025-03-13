#pragma once

#include <vulkan/vulkan.h>

struct Pipeline {
    VkPipeline raw = VK_NULL_HANDLE;
    // Can share layout between different pipelines
    VkPipelineLayout* layout = nullptr;
};