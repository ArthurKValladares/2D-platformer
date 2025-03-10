#include "texture.h"

#include "buffer.h"
#include "renderer.h"
#include "initializers.h"
#include "tools.h"

#include <cassert>

Texture::Texture(Renderer* renderer, TextureCreateInfo ci) {
    assert(ci.buffer);

    Buffer staging_buffer = Buffer(
        renderer->allocator,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        ci.buffer,
        ci.buffer_size
    );

    this->width = ci.width;
    this->height = ci.height;
    this->mip_levels = 1;

    VkImageCreateInfo image_create_info = initializers::image_create_info();
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = ci.format;
    image_create_info.mipLevels = mip_levels;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.extent = {
        .width = width, 
        .height = height,
        .depth = 1
    };
    image_create_info.usage = 
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    VmaAllocationCreateInfo image_alloc_ci = {
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    chk(vmaCreateImage(renderer->allocator, &image_create_info, &image_alloc_ci, &image, &img_allocation, nullptr));

    VkCommandBuffer copy_cmd = renderer->create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkImageSubresourceRange subresource_range = {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = mip_levels,
        .layerCount = 1
    };

    tools::set_image_layout(
        copy_cmd,
        image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        subresource_range
    );

    VkBufferImageCopy region{
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {0, 0, 0},
        .imageExtent = {
            width,
            height,
            1
        }
    };
    vkCmdCopyBufferToImage(
        copy_cmd,
        staging_buffer.raw,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    this->image_layout = ci.image_layout;
    tools::set_image_layout(
        copy_cmd,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        image_layout,
        subresource_range
    );

    VkQueue copy_queue = renderer->get_graphics_queue();
    renderer->flush_command_buffer(copy_cmd, copy_queue, true);
   
    staging_buffer.destroy(renderer->allocator);

    VkImageViewCreateInfo view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = ci.format,
        .components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
        .subresourceRange = { 
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = mip_levels,
            .baseArrayLayer = 0,
            .layerCount = 1
        },
    };
    chk(vkCreateImageView(renderer->device, &view_create_info, nullptr, &view));

    VkSamplerCreateInfo sampler_create_info = initializers::sampler_create_info();
    sampler_create_info.magFilter = ci.mag_filter;
    sampler_create_info.minFilter = ci.min_filter;
    sampler_create_info.mipmapMode = ci.mipmap_mode;
    sampler_create_info.addressModeU = ci.address_mode_u;
    sampler_create_info.addressModeV = ci.address_mode_v;
    sampler_create_info.addressModeW = ci.address_mode_v;
    sampler_create_info.mipLodBias = 0.0f;
    sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
    sampler_create_info.minLod = 0.0f;
    sampler_create_info.maxLod = (float) mip_levels;
    sampler_create_info.maxAnisotropy = renderer->properties.limits.maxSamplerAnisotropy;
    sampler_create_info.anisotropyEnable = renderer->enabled_features.samplerAnisotropy;
    chk(vkCreateSampler(renderer->device, &sampler_create_info, nullptr, &sampler));

    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = image_layout;
}

void Texture::destroy(Renderer* renderer) {
    vkDestroySampler(renderer->device, sampler, nullptr);
    vkDestroyImageView(renderer->device, view, nullptr);

    vmaDestroyImage(renderer->allocator, image, img_allocation);
}