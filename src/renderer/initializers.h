#pragma once

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
};