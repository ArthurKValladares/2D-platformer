#pragma once

#include <stdint.h>

#include "renderer/resource_ids.h"

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
TextureID texture_id(TextureSource texture);

enum class ShaderSource : uint32_t {
    TriangleFrag = 0,
    TriangleVert,
    TriangleTransformVert,
    Count
};
const char* shader_path(ShaderSource shader);
ShaderID shader_id(ShaderSource shader);
//
//
//