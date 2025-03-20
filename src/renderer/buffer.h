#pragma once

#include <vector>

#include <vma/vk_mem_alloc.h>

#include "../util.h"

struct Buffer {
    Buffer() {}

    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, VmaMemoryUsage vma_usage, uint64_t size_bytes) {
        size_bytes = size_bytes;
        VkBufferCreateInfo buffer_ci = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size_bytes,
            .usage = usage
        };
        VmaAllocationCreateInfo buffer_alloc_ci = {
            .flags = allocation_flags,
            .usage = vma_usage
        };
        chk(vmaCreateBuffer(allocator, &buffer_ci, &buffer_alloc_ci, &raw, &allocation, nullptr));

        descriptor.buffer = raw;
        descriptor.offset = 0;
        descriptor.range = size_bytes;
    }

    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, VmaMemoryUsage vma_usage, void* data, uint64_t size_bytes)
        : Buffer(allocator, usage, allocation_flags, vma_usage, size_bytes)
    {
        void* buffer_ptr = nullptr;
        vmaMapMemory(allocator, allocation, &buffer_ptr);
        memcpy(buffer_ptr, data, size_bytes);
        vmaUnmapMemory(allocator, allocation);
    }

    template<class T>
    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, VmaMemoryUsage vma_usage, const std::vector<T>& data)
        : Buffer(allocator, usage, allocation_flags, vma_usage, data.data(), data.size())
    {}

    void destroy(VmaAllocator allocator);

    VmaAllocation allocation = VK_NULL_HANDLE;
    VkBuffer raw = VK_NULL_HANDLE;
    uint64_t size_bytes = 0;
    VkDescriptorBufferInfo descriptor;
};