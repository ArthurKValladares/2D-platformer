#pragma once

#include <string>

struct ImageData {
    ImageData() {}
    ImageData(const std::string& path);

    ~ImageData();

    int width, height, channels;
    unsigned char* img;
    size_t size;
};