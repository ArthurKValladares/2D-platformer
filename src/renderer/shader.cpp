#include "shader.h"

#include <cassert>

namespace {
    VkDescriptorType from_spv(SpvReflectDescriptorType spv) {
        return static_cast<VkDescriptorType>(static_cast<uint32_t>(spv));
    }
    VkShaderStageFlagBits from_spv(SpvReflectShaderStageFlagBits spv) {
        return static_cast<VkShaderStageFlagBits>(static_cast<uint32_t>(spv));
    }
};

ShaderData::ShaderData(VkDevice device, size_t size, const void* p_code) {
    this->device = device;

    VkShaderModuleCreateInfo shader_module_ci = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = size,
        .pCode = (const uint32_t*) p_code
    };
    vkCreateShaderModule(device, &shader_module_ci, nullptr, &shader_module);

    // Generate reflection data for a shader
    SpvReflectResult result = spvReflectCreateShaderModule(size, p_code, &spv_module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    // Enumerate and extract shader's input variables
    uint32_t var_count = 0;
    result = spvReflectEnumerateInputVariables(&spv_module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        input_vars.resize(var_count);
        result = spvReflectEnumerateInputVariables(&spv_module, &var_count, &input_vars[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }

    // Enumerate and extract shader's output variables
    var_count = 0;
    result = spvReflectEnumerateOutputVariables(&spv_module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        output_vars.resize(var_count);
        result = spvReflectEnumerateOutputVariables(&spv_module, &var_count, &output_vars[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }

    // Enumerate and extract shader's Descriptor bindings
    var_count = 0;
    result = spvReflectEnumerateDescriptorBindings(&spv_module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        descriptor_bindings.resize(var_count);
        result = spvReflectEnumerateDescriptorBindings(&spv_module, &var_count, &descriptor_bindings[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }
    
    // Enumerate and extract shader's Descriptor sets
    var_count = 0;
    result = spvReflectEnumerateDescriptorSets(&spv_module, &var_count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);
    if (var_count != 0) {
        descriptor_sets.resize(var_count);
        result = spvReflectEnumerateDescriptorSets(&spv_module, &var_count, &descriptor_sets[0]);
        assert(result == SPV_REFLECT_RESULT_SUCCESS);
    }
}

ShaderData::~ShaderData() {
    spvReflectDestroyShaderModule(&spv_module);
    vkDestroyShaderModule(device, shader_module, nullptr);
}

VkShaderStageFlags ShaderData::shader_stage() const {
    return static_cast<VkShaderStageFlags>(spv_module.shader_stage);
}

VkShaderStageFlagBits ShaderData::shader_stage_bits() const {
    return from_spv(spv_module.shader_stage);
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
            .stageFlags = shader_stage()
        });
    }

    return bindings;
}