#pragma once

#include <vulkan/vulkan.h>

#include <span>

namespace initializers {
    inline VkImageCreateInfo image_create_info() {
        return VkImageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO
        };
    }

    inline VkImageMemoryBarrier image_memory_barrier() {
        return VkImageMemoryBarrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
        };
    }

    inline VkCommandBufferAllocateInfo command_buffer_allocate_info(
        VkCommandPool command_pool,
        VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        uint32_t buffer_count = 1)
    {
        return VkCommandBufferAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool,
            .level = level,
            .commandBufferCount = buffer_count,
        };
    }

    inline VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0) {
        return VkCommandBufferBeginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = flags,
            .pInheritanceInfo = nullptr
        };
    }

    inline VkCommandBufferSubmitInfo command_buffer_submit_info(VkCommandBuffer cmd) {
        return VkCommandBufferSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .pNext = nullptr,
            .commandBuffer = cmd,
            .deviceMask = 0,
        };        
    }

    inline VkSubmitInfo2 submit_info(
        VkCommandBufferSubmitInfo* cmd,
        VkSemaphoreSubmitInfo* signal_semaphore_info,
        VkSemaphoreSubmitInfo* wait_semaphore_info
    ) {
        return VkSubmitInfo2 {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .pNext = nullptr,
            .waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? (uint32_t) 0 : (uint32_t) 1,
            .pWaitSemaphoreInfos = wait_semaphore_info,
            .commandBufferInfoCount = (uint32_t) 1,
            .pCommandBufferInfos = cmd,
            .signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? (uint32_t) 0 : (uint32_t) 1,
            .pSignalSemaphoreInfos = signal_semaphore_info
        };
    }

    inline VkSubmitInfo submit_info() {
        return VkSubmitInfo {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO
        };
    }

    inline VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0) {
        return VkFenceCreateInfo {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = flags
        };
    }

    inline VkSamplerCreateInfo sampler_create_info() {
        return VkSamplerCreateInfo {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .maxAnisotropy = 1.0f
        };
    }

    inline VkDescriptorSetLayoutCreateInfo descriptor_set_create_info(uint32_t binding_count = 0, const VkDescriptorSetLayoutBinding* p_bindings = nullptr, VkDescriptorSetLayoutCreateFlags flags = {}) {
        return VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags = flags,
            .bindingCount = binding_count,
            .pBindings = p_bindings
        };
    }
    inline VkDescriptorSetLayoutCreateInfo descriptor_set_create_info(std::span<const VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags = {}) {
        return descriptor_set_create_info((uint32_t) bindings.size(), bindings.data(), flags);
    }

    inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(uint32_t set_layout_count = 0, const VkDescriptorSetLayout* set_layouts = nullptr) {
        return VkPipelineLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = set_layout_count,
            .pSetLayouts = set_layouts
        };
    }
    inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(std::span<const VkDescriptorSetLayout> set_layouts) {
        return pipeline_layout_create_info((uint32_t) set_layouts.size(), set_layouts.data());
    }
    inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(const VkDescriptorSetLayout& set_layouts) {
        return pipeline_layout_create_info(1, &set_layouts);
    }

    inline VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info(VkShaderStageFlagBits stage, VkShaderModule module) {
        return VkPipelineShaderStageCreateInfo {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .stage = stage,
            .module = module,
            .pName = "main"
        };
    }

    inline VkRenderingAttachmentInfo rendering_attachment_info(VkImageView view, VkImageLayout layout, VkImageView resolve_image_view, const VkClearValue* clear = nullptr) {
        VkRenderingAttachmentInfo color_attachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = view,
            .imageLayout = layout,
            .resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
            .resolveImageView = resolve_image_view,
            .resolveImageLayout = VK_IMAGE_LAYOUT_GENERAL,
            .loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };

        if (clear) {
            color_attachment.clearValue = *clear;
        }
    
        return color_attachment;
    }

    inline VkRenderingInfo rendering_info(VkExtent2D render_extent, VkRenderingAttachmentInfo* color_attachment, VkRenderingAttachmentInfo* depth_attachment = nullptr)
    {
        return VkRenderingInfo {
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                .extent = render_extent
            },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = color_attachment,
            .pDepthAttachment = depth_attachment,
            .pStencilAttachment = nullptr,
        };
    }
    
};