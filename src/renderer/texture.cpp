#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "buffer.h"
#include "renderer.h"
#include "initializers.h"

ImageData::ImageData(const std::string& path) {
    img = stbi_load(path.c_str(), &width, &height, &channels, 4);
    size = static_cast<uint32_t>(width * height * 4);
    assert(img != nullptr);
}

ImageData::~ImageData() {
    stbi_image_free(img);
}

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

    VkImageCreateInfo image_create_info = initializers::image_create_info();
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = ci.format;
    image_create_info.mipLevels = 1;
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

    /*
    VkCommandBuffer copy_cmd = renderer->create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

    VkQueue copy_queue = renderer->get_graphics_queue();
    renderer->flush_command_buffer(copy_cmd, copy_queue, true);
    */
   
    staging_buffer.destroy(renderer->allocator);
}

void Texture::destroy(VmaAllocator allocator) {
    vmaDestroyImage(allocator, image, img_allocation);
}