#include "shader.h"

#include <cassert>

namespace {
    VkDescriptorType from_spv(SpvReflectDescriptorType spv) {
        return static_cast<VkDescriptorType>(static_cast<uint32_t>(spv));
    }
    VkShaderStageFlags from_spv(SpvReflectShaderStageFlagBits spv) {
        return static_cast<VkShaderStageFlags>(static_cast<uint32_t>(spv));
    }
};

ShaderData::ShaderData(size_t size, const void* p_code) {
    // Generate reflection data for a shader
    SpvReflectResult result = spvReflectCreateShaderModule(size, p_code, &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Enumerate and extract shader's input variables
    uint32_t var_count = 0;
    result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        input_vars.resize(var_count);
        result = spvReflectEnumerateInputVariables(&module, &var_count, &input_vars[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }

    // Enumerate and extract shader's output variables
    var_count = 0;
    result = spvReflectEnumerateOutputVariables(&module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        output_vars.resize(var_count);
        result = spvReflectEnumerateOutputVariables(&module, &var_count, &output_vars[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }

    // Enumerate and extract shader's Descriptor bindings
    var_count = 0;
    result = spvReflectEnumerateDescriptorBindings(&module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        descriptor_bindings.resize(var_count);
        result = spvReflectEnumerateDescriptorBindings(&module, &var_count, &descriptor_bindings[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }
    
    // Enumerate and extract shader's Descriptor sets
    var_count = 0;
    result = spvReflectEnumerateDescriptorSets(&module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        descriptor_sets.resize(var_count);
        result = spvReflectEnumerateDescriptorSets(&module, &var_count, &descriptor_sets[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }
}

ShaderData::~ShaderData() {
    spvReflectDestroyShaderModule(&module);
}

std::vector<VkDescriptorSetLayoutBinding> ShaderData::get_layout_bindings(uint32_t set_idx) const {
    const SpvReflectDescriptorSet* set = descriptor_sets[set_idx];
    const uint32_t binding_count = set->binding_count;

    std::vector<VkDescriptorSetLayoutBinding> bindings;
    bindings.reserve(binding_count);

    for (uint64_t i = 0; i < binding_count; ++i) {
        const SpvReflectDescriptorBinding* spv_binding = set->bindings[i];

        bindings.push_back(VkDescriptorSetLayoutBinding{
            .binding = spv_binding->binding,
            .descriptorType = from_spv(spv_binding->descriptor_type),
            .descriptorCount = spv_binding->count,
            .stageFlags = from_spv(module.shader_stage)
        });
    }

    return bindings;
}