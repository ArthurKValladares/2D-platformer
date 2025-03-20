#pragma once

#include <vector>

#include <vma/vk_mem_alloc.h>

#include "../util.h"

struct Buffer {
    Buffer() {}

    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, uint64_t size_bytes) {
        size_bytes = size_bytes;
        VkBufferCreateInfo buffer_ci = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size_bytes,
            .usage = usage
        };
        VmaAllocationCreateInfo buffer_alloc_ci = {
            .flags = allocation_flags,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        chk(vmaCreateBuffer(allocator, &buffer_ci, &buffer_alloc_ci, &raw, &allocation, nullptr));

        descriptor.buffer = raw;
        descriptor.offset = 0;
        descriptor.range = size_bytes;
    }

    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, void* data, uint64_t size_bytes)
        : Buffer(allocator, usage, allocation_flags, size_bytes)
    {
        void* buffer_ptr = nullptr;
        vmaMapMemory(allocator, allocation, &buffer_ptr);
        memcpy(buffer_ptr, data, size_bytes);
        vmaUnmapMemory(allocator, allocation);
    }

    template<class T>
    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, T* data, uint64_t length)
        : Buffer(allocator, usage, allocation_flags, (void*) data, length * sizeof(T))
    {}
    template<class T>
    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, VmaAllocationCreateFlags allocation_flags, const std::vector<T>& data)
        : Buffer(allocator, usage, allocation_flags, data.data(), data.size())
    {}

    void destroy(VmaAllocator allocator);

    VmaAllocation allocation = VK_NULL_HANDLE;
    VkBuffer raw = VK_NULL_HANDLE;
    uint64_t size_bytes = 0;
    VkDescriptorBufferInfo descriptor;
};