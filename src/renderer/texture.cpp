#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "buffer.h"

ImageData::ImageData(const std::string& path) {
    img = stbi_load(path.c_str(), &width, &height, &channels, 4);
    size = static_cast<uint32_t>(width * height * 4);
    assert(img != nullptr);
}

ImageData::~ImageData() {
    stbi_image_free(img);
}

Texture::Texture(VmaAllocator allocator, TextureCreateInfo ci) {
    assert(ci.buffer);

    Buffer staging_buffer = Buffer(
        allocator,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
        ci.buffer,
        ci.buffer_size
    );

    staging_buffer.destroy(allocator);
}

Texture::~Texture() {
}