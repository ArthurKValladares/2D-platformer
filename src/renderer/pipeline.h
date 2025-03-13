#pragma once

#include <vulkan/vulkan.h>

#include "shader.h"

struct Pipeline {
    Pipeline() {}
    // TODO: In the future when i want to hash these params and have a pipeline-map
    // I should use the enum as the key for the shaders
    Pipeline(
        VkDevice device,
        VkPipelineLayout* layout,
        const ShaderData& vert_shader_data, const ShaderData& frag_shader_data,
        VkSampleCountFlagBits sample_count,
        uint32_t color_attachment_count, const VkFormat* p_color_attachment_formats
    );
    Pipeline(
        VkDevice device,
        VkPipelineLayout* layout,
        const ShaderData& vert_shader_data, const ShaderData& frag_shader_data,
        VkSampleCountFlagBits sample_count,
        const std::vector<VkFormat> color_attachment_formats
    )
        : Pipeline(device, layout, vert_shader_data, frag_shader_data, sample_count,
            (uint32_t) color_attachment_formats.size(), color_attachment_formats.data())
    {}
    Pipeline(
        VkDevice device,
        VkPipelineLayout* layout,
        const ShaderData& vert_shader_data, const ShaderData& frag_shader_data,
        VkSampleCountFlagBits sample_count,
        const VkFormat& color_attachment_format
    )
        : Pipeline(device, layout, vert_shader_data, frag_shader_data, sample_count,
            1, &color_attachment_format)
    {}

    void destroy(VkDevice device);

    VkPipeline raw = VK_NULL_HANDLE;
    VkPipelineLayout* layout = nullptr;
};