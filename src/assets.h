#pragma once

#include <stdint.h>

//
// TODO: These should be auto-generated later
//
enum class TextureSource : uint32_t {
    Test1 = 0,
    Test2,
    Test3,
    Test4,
    Count
};
const char* texture_path(TextureSource texture);

enum class ShaderSource : uint32_t {
    TriangleFrag = 0,
    TriangleVert,
    Count
};
const char* shader_path(ShaderSource shader);
//
//
//