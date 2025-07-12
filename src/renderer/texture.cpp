#include "texture.h"

#include "buffer.h"
#include "renderer.h"
#include "initializers.h"

#include <cassert>

Texture::Texture(Renderer* renderer, TextureCreateInfo ci) {
    assert(ci.buffer);

    Buffer staging_buffer = Buffer(
        renderer->allocator,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        VMA_MEMORY_USAGE_AUTO,
        ci.buffer,
        ci.buffer_size
    );

    this->width = ci.width;
    this->height = ci.height;

    const VkExtent3D image_extent = initializers::extent_3D(width, height);
    const VkImageCreateInfo image_create_info = initializers::image_create_info(
        ci.format,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        image_extent
    );
    const VmaAllocationCreateInfo image_alloc_ci = {
        .usage = VMA_MEMORY_USAGE_AUTO
    };
    chk(vmaCreateImage(renderer->allocator, &image_create_info, &image_alloc_ci, &image, &img_allocation, nullptr));

    const VkImageSubresourceRange subresource_range = initializers::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT);
    renderer->immediate_submit([&](VkCommandBuffer cmd) {
        tools::set_image_layout(
            cmd,
            image,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresource_range
        );

        const VkImageSubresourceLayers layer = initializers::image_subresource_layers(VK_IMAGE_ASPECT_COLOR_BIT, 0);
        const VkBufferImageCopy region = initializers::buffer_image_copy(layer, image_extent);
        vkCmdCopyBufferToImage(
            cmd,
            staging_buffer.raw,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        this->image_layout = ci.image_layout;
        tools::set_image_layout(
            cmd,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            image_layout,
            subresource_range
        );
    });
   
    staging_buffer.destroy(renderer->allocator);

    const VkImageViewCreateInfo view_create_info = initializers::image_view_create_info(ci.format, image, subresource_range);
    chk(vkCreateImageView(renderer->device, &view_create_info, nullptr, &view));

    VkSamplerCreateInfo sampler_create_info = initializers::sampler_create_info(ci.mag_filter, ci.min_filter, ci.mipmap_mode, ci.address_mode_u, ci.address_mode_v, ci.address_mode_w);    
    chk(vkCreateSampler(renderer->device, &sampler_create_info, nullptr, &sampler));

    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = image_layout;
}

void Texture::destroy(Renderer* renderer) {
    if (!is_empty()) {
        vkDestroySampler(renderer->device, sampler, nullptr);
        vkDestroyImageView(renderer->device, view, nullptr);

        vmaDestroyImage(renderer->allocator, image, img_allocation);
    }
}