#include "tools.h"

#include "initializers.h"

void tools::set_image_layout(
    VkCommandBuffer cmd_buffer,
    VkImage image,
    VkImageLayout old_image_layout,
    VkImageLayout new_image_layout,
    VkImageSubresourceRange subresource_range,
    VkPipelineStageFlags src_stage_mask,
    VkPipelineStageFlags dst_stage_mask)
{
    VkImageMemoryBarrier image_memory_barrier = initializers::image_memory_barrier();
    image_memory_barrier.oldLayout = old_image_layout;
    image_memory_barrier.newLayout = new_image_layout;
    image_memory_barrier.image = image;
    image_memory_barrier.subresourceRange = subresource_range;

    // Source layouts (old)
    // Source access mask controls actions that have to be finished on the old layout
    // before it will be transitioned to the new layout
    switch (old_image_layout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED: {
        // Image layout is undefined (or does not matter)
        // Only valid as initial layout
        // No flags required, listed only for completeness
        image_memory_barrier.srcAccessMask = 0;
        break;
    }
    case VK_IMAGE_LAYOUT_PREINITIALIZED: {
        // Image is preinitialized
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes have been finished
        image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
        // Image is a color attachment
        // Make sure any writes to the color buffer have been finished
        image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: {
        // Image is a depth/stencil attachment
        // Make sure any writes to the depth/stencil buffer have been finished
        image_memory_barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: {
        // Image is a transfer source 
        // Make sure any reads from the image have been finished
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
        // Image is a transfer destination
        // Make sure any writes to the image have been finished
        image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
        // Image is read by a shader
        // Make sure any shader reads from the image have been finished
        image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    }
    default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Target layouts (new)
    // Destination access mask controls the dependency for the new image layout
    switch (new_image_layout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: {
        // Image will be used as a transfer destination
        // Make sure any writes to the image have been finished
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: {
        // Image will be used as a transfer source
        // Make sure any reads from the image have been finished
        image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: {
        // Image will be used as a color attachment
        // Make sure any writes to the color buffer have been finished
        image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: {
        // Image layout will be used as a depth/stencil attachment
        // Make sure any writes to depth/stencil buffer have been finished
        image_memory_barrier.dstAccessMask = image_memory_barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    }
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
        // Image will be read in a shader (sampler, input attachment)
        // Make sure any writes to the image have been finished
        if (image_memory_barrier.srcAccessMask == 0)
        {
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;
    }
    default:
        // Other source layouts aren't handled (yet)
        break;
    }

    // Put barrier inside setup command buffer
    vkCmdPipelineBarrier(
        cmd_buffer,
        src_stage_mask,
        dst_stage_mask,
        0,
        0, nullptr,
        0, nullptr,
        1, &image_memory_barrier);
}
    

void tools::set_image_layout(
    VkCommandBuffer cmd_buffer,
    VkImage image,
    VkImageAspectFlags aspect_mask,
    VkImageLayout old_image_layout,
    VkImageLayout new_image_layout,
    VkPipelineStageFlags src_stage_mask,
    VkPipelineStageFlags dst_stage_mask)
{
    VkImageSubresourceRange subresource_range = {};
    subresource_range.aspectMask = aspect_mask;
    subresource_range.baseMipLevel = 0;
    subresource_range.levelCount = 1;
    subresource_range.layerCount = 1;
    set_image_layout(cmd_buffer, image, old_image_layout, new_image_layout, subresource_range, src_stage_mask, dst_stage_mask);
}  