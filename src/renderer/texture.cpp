#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ImageData::ImageData(const std::string& path) {
    img = stbi_load(path.c_str(), &width, &height, &channels, 4);
    img_size = static_cast<uint32_t>(width * height * 4);
    assert(img != nullptr);
}