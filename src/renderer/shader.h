#pragma once

#include <vector>

#include "spirv_reflect.h"

struct ShaderData {
    ShaderData(size_t size, const void* p_code);
    ~ShaderData();

    SpvReflectShaderModule module;
    std::vector<SpvReflectInterfaceVariable*> input_vars;
    std::vector<SpvReflectInterfaceVariable*> output_vars;
    std::vector<SpvReflectDescriptorBinding*> descriptor_bindings;
    std::vector<SpvReflectDescriptorSet*> descriptor_sets;
};