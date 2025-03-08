#pragma once

#include <string>

#include <vma/vk_mem_alloc.h>

struct ImageData {
    ImageData() {}
    ImageData(const std::string& path);

    int width, height, channels;
    unsigned char* img;
    size_t img_size;
};

struct Texture {
    VmaAllocation img_allocation;
    VkImage image = VK_NULL_HANDLE;
    VkImageLayout image_layout;
    VkDeviceMemory device_memory;
    VkImageView view;
    uint32_t width, height;
    uint32_t mip_levels;
    uint32_t layer_count;
    VkDescriptorImageInfo descriptor;
    VkSampler sampler;
};