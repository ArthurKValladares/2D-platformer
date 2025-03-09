#pragma once

#include <string>

#include <vma/vk_mem_alloc.h>

struct ImageData {
    ImageData() {}
    ImageData(const std::string& path);

    ~ImageData();

    int width, height, channels;
    unsigned char* img;
    size_t size;
};

struct TextureCreateInfo {
    void* buffer;
    VkDeviceSize buffer_size;
    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_SAMPLED_BIT;
    VkImageLayout image_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    bool create_sampler = true;
    VkFilter mag_filter = VK_FILTER_LINEAR;
    VkFilter min_filter = VK_FILTER_LINEAR;
    VkSamplerMipmapMode mipmap_mode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    VkSamplerAddressMode address_mode_u = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode address_mode_v = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode address_mode_w = VK_SAMPLER_ADDRESS_MODE_REPEAT;
};

struct Renderer;
struct Texture {
    Texture() {}
    Texture(Renderer* renderer, TextureCreateInfo ci);

    void destroy(VmaAllocator allocator);

    VmaAllocation img_allocation;
    VkImage image;
    VkImageLayout image_layout;
    VkDeviceMemory device_memory;
    VkImageView view;
    uint32_t width, height;
    VkDescriptorImageInfo descriptor;
    VkSampler sampler;
};