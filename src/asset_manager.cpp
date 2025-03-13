#include "asset_manager.h"

#include <cassert>

const char* texture_path(TextureSource texture) {
    switch (texture)
    {
        case TextureSource::Test1: {
            return "assets/textures/test_1.png";
        }
        case TextureSource::Test2: {
            return "assets/textures/test_2.png";
        }
        case TextureSource::Test3: {
            return "assets/textures/test_3.png";
        }
        case TextureSource::Test4: {
            return "assets/textures/test_4.png";
        }
        case TextureSource::Count: {
            assert(false);
            return "";
        }
    }
}

TextureManager::TextureManager() {
    const uint64_t count = static_cast<uint64_t>(TextureSource::Count);
    images.reserve(count);
    for (uint64_t i = 0; i < count; ++i) {
        images.emplace_back(texture_path(static_cast<TextureSource>(i)));
    }
}

const char* shader_path(ShaderSource shader) {
    switch (shader)
    {
        case ShaderSource::TriangleFrag: {
            return "shaders/triangle.frag.spv";
        }
        case ShaderSource::TriangleVert: {
            return "shaders/triangle.vert.spv";
        }
        case ShaderSource::Count: {
            assert(false);
            return "";
        }
    }
}