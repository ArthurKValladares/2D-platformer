#include "shader.h"

#include <cassert>

ShaderData::ShaderData(size_t size, const void* p_code) {
    // Generate reflection data for a shader
    SpvReflectResult result = spvReflectCreateShaderModule(size, p_code, &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Enumerate and extract shader's input variables
    uint32_t var_count = 0;
    result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    interface_variables.resize(var_count);
    result = spvReflectEnumerateInputVariables(&module, &var_count, &interface_variables[0]);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
}

ShaderData::~ShaderData() {
    spvReflectDestroyShaderModule(&module);
}