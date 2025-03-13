#pragma once

#include <vulkan/vulkan.h>

#include <vector>

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
        VkCommandBufferLevel level, 
        uint32_t buffer_count)
    {
        return VkCommandBufferAllocateInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = command_pool,
            .level = level,
            .commandBufferCount = buffer_count,
        };
    }

    inline VkCommandBufferBeginInfo command_buffer_begin_info() {
        return VkCommandBufferBeginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
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

    inline VkDescriptorSetLayoutCreateInfo descriptor_set_create_info(uint32_t binding_count = 0, const VkDescriptorSetLayoutBinding* p_bindings = nullptr) {
        return VkDescriptorSetLayoutCreateInfo {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = binding_count,
            .pBindings = p_bindings
        };
    }
    inline VkDescriptorSetLayoutCreateInfo descriptor_set_create_info(const std::vector<VkDescriptorSetLayoutBinding>& bindings) {
        return descriptor_set_create_info((uint32_t) bindings.size(), bindings.data());
    }
};