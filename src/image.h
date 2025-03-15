#pragma once

#include <string>
#include "renderer/texture.h"

struct ImageData {
    ImageData() {}
    ImageData(const std::string& path);

    ~ImageData();

    TextureCreateInfo texture_create_info() const;

    int width, height, channels;
    unsigned char* img;
    size_t size;
};