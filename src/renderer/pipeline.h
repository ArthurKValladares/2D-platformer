#pragma once

#include <vulkan/vulkan.h>

#include "shader.h"

class Renderer;
struct Pipeline {
    Pipeline() {}
    // TODO: In the future when i want to hash these params and have a pipeline-map
    // I should use the enum as the key for the shaders
    Pipeline(
        const Renderer& renderer,
        std::pair<uint32_t, uint32_t> layout_id,
        VkSampleCountFlagBits sample_count,
        uint32_t color_attachment_count, const VkFormat* p_color_attachment_formats
    );
    Pipeline(
        const Renderer& renderer,
        std::pair<uint32_t, uint32_t> layout_id,
        VkSampleCountFlagBits sample_count,
        const std::vector<VkFormat> color_attachment_formats
    )
        : Pipeline(renderer, layout_id, sample_count,
            (uint32_t) color_attachment_formats.size(), color_attachment_formats.data())
    {}
    Pipeline(
        const Renderer& renderer,
        std::pair<uint32_t, uint32_t> layout_id,
        VkSampleCountFlagBits sample_count,
        const VkFormat& color_attachment_format
    )
        : Pipeline(renderer, layout_id, sample_count,
            1, &color_attachment_format)
    {}

    void destroy(VkDevice device);

    VkPipeline raw;
    std::pair<uint32_t, uint32_t> layout_id;
};