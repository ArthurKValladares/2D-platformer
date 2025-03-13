#pragma once

#include <vector>

#include "spirv_reflect.h"

struct ShaderData {
    ShaderData(size_t size, const void* p_code);
    ~ShaderData();
    
    SpvReflectShaderModule module;
    std::vector<SpvReflectInterfaceVariable*> interface_variables;
};