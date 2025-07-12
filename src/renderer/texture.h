#pragma once

#include <vma/vk_mem_alloc.h>

struct TextureCreateInfo {
    void* buffer;
    VkDeviceSize buffer_size;
    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_SAMPLED_BIT;
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
    Texture() 
        : image(VK_NULL_HANDLE)
    {}
    Texture(Renderer* renderer, TextureCreateInfo ci);

    void destroy(Renderer* renderer);

    bool is_empty() const {
        return image == VK_NULL_HANDLE;
    }
    
    uint32_t width, height;

    VmaAllocation img_allocation;
    VkImage image;
    VkImageLayout image_layout;
    VkImageView view;
    VkDescriptorImageInfo descriptor;
    VkSampler sampler;
};