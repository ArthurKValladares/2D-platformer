#include "shader.h"

#include <cassert>

#define SPIRV_BYTE_WIDTH 8

namespace {
    VkDescriptorType from_spv(SpvReflectDescriptorType spv) {
        return static_cast<VkDescriptorType>(static_cast<uint32_t>(spv));
    }
    VkShaderStageFlagBits from_spv(SpvReflectShaderStageFlagBits spv) {
        return static_cast<VkShaderStageFlagBits>(static_cast<uint32_t>(spv));
    }
    VkFormat from_spv(SpvReflectFormat spv) {
        return static_cast<VkFormat>(static_cast<uint32_t>(spv));
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

uint32_t ShaderData::max_descriptor_set() const {
    uint32_t max = 0;
    for (SpvReflectDescriptorSet* set : descriptor_sets) {
        if (set->set > max) {
            max = set->set;
        }
    }
    return max;
}

void ShaderData::append_layout_bindings(BindingsMap& bindings_map) const {
    for (SpvReflectDescriptorSet* set : descriptor_sets) {
        const uint32_t set_idx = set->set;
        const uint32_t binding_count = set->binding_count;

        std::vector<VkDescriptorSetLayoutBinding>& bindings = bindings_map[set_idx];
        
        for (uint64_t i = 0; i < binding_count; ++i) {
            const SpvReflectDescriptorBinding* spv_binding = set->bindings[i];
    
            bindings.push_back(VkDescriptorSetLayoutBinding{
                .binding = spv_binding->binding,
                .descriptorType = from_spv(spv_binding->descriptor_type),
                .descriptorCount = spv_binding->count,
                .stageFlags = shader_stage()
            });
        }
    }
}

void ShaderData::get_vertex_input_data(
    VkVertexInputBindingDescription* binding_desc,
    std::vector<VkVertexInputAttributeDescription>& attribute_description,  
    VkPipelineVertexInputStateCreateInfo* ci) const
{
    assert(shader_stage_bits() == VK_SHADER_STAGE_VERTEX_BIT);
    // TODO: Atm hard-coded to one vertex input binding

    uint32_t offset = 0;
    for (const SpvReflectInterfaceVariable* var : input_vars) {
       attribute_description.push_back(VkVertexInputAttributeDescription{
            .location = var->location,
            .binding = 0,
            .format = from_spv(var->format),
            .offset = offset
       });

        uint32_t size = 0;
        switch (var->type_description->op) {
            case SpvOpTypeVector: {
                size =
                    var->numeric.vector.component_count * (var->numeric.scalar.width / SPIRV_BYTE_WIDTH);
                break;
            };
            default: {
                assert(false);
                break;
            }
        }

        offset += size;
    }

    *binding_desc = VkVertexInputBindingDescription {
        .binding = 0,
        .stride = offset,
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
   
    *ci = VkPipelineVertexInputStateCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = binding_desc,
        .vertexAttributeDescriptionCount = (uint32_t) attribute_description.size(),
        .pVertexAttributeDescriptions = attribute_description.data(),
    };
}