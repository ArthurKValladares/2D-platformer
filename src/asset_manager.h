#pragma once

#include <stdint.h>
#include <vector>

#include "image.h"

// TODO: This stuff should all be auto-generated at some point
enum class TextureSource : uint32_t {
    Test1 = 0,
    Test2,
    Test3,
    Test4,
    Count
};

const char* texture_path(TextureSource texture);

struct TextureManager {
    TextureManager();

    std::vector<ImageData> images;
};