#pragma once

#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <utility>

#include "spirv_reflect.h"

#include "image.h"
#include "renderer/pipeline.h"

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

struct TextureManager {
    TextureManager();

    // TODO: Holding on to this data forerver is bad, i only need (most of it) it to create the GPU textures
    std::vector<ImageData> images;
};

struct PipelineManager {
    std::unordered_map<std::pair<ShaderSource, ShaderSource>, Pipeline> pipelines;
};