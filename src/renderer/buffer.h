#pragma once

#include <vector>

#include <vma/vk_mem_alloc.h>

struct Buffer {
    Buffer() {}

    template<class T>
    Buffer(VmaAllocator allocator, VkBufferUsageFlags usage, const std::vector<T>& data) {
        const uint64_t size_bytes = data.size() * sizeof(T);
        VkBufferCreateInfo buffer_ci = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size_bytes,
            .usage = usage
        };
        VmaAllocationCreateInfo buffer_alloc_ci = {
            .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_AUTO
        };
        chk(vmaCreateBuffer(allocator, &buffer_ci, &buffer_alloc_ci, &raw, &allocation, nullptr));
        void* buffer_ptr = nullptr;
        vmaMapMemory(allocator, allocation, &buffer_ptr);
        memcpy(buffer_ptr, (void*) data.data(), size_bytes);
        vmaUnmapMemory(allocator, allocation);

        length = data.size();
    }

    void destroy(VmaAllocator allocator);

    VmaAllocation allocation = VK_NULL_HANDLE;
    VkBuffer raw = VK_NULL_HANDLE;
    uint64_t size_bytes = 0;
    uint64_t length = 0;
};