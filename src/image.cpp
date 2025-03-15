#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageData::ImageData(const std::string& path) {
    img = stbi_load(path.c_str(), &width, &height, &channels, 4);
    size = static_cast<uint32_t>(width * height * 4);
    assert(img != nullptr);
}

ImageData::~ImageData() {
    stbi_image_free(img);
}

TextureCreateInfo ImageData::texture_create_info() const {
    return TextureCreateInfo{
        .buffer = img,
        .buffer_size = size,
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
        .format = VK_FORMAT_R8G8B8A8_SRGB,
    };
}