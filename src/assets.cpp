#include "assets.h"

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
        default:{
            assert(false);
            return "";
        }
    }
}

TextureID texture_id(TextureSource texture) {
    return TextureID(static_cast<uint32_t>(texture));
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
        case ShaderSource::TriangleTransformVert: {
            return "shaders/triangle_transform.vert.spv";
        }
        case ShaderSource::Count: {
            assert(false);
            return "";
        }
        default:{
            assert(false);
            return "";
        }
    }
}

ShaderID shader_id(ShaderSource shader) {
    return ShaderID(static_cast<uint32_t>(shader));
}