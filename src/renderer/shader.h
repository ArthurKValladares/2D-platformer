#pragma once

#include <vector>

#include <vulkan/vulkan.h>
#include "spirv_reflect.h"

// TODO: could just be a vector later
using BindingsMap = std::vector<std::vector<VkDescriptorSetLayoutBinding>>;

struct ShaderData {
    ShaderData() {}
    ShaderData(VkDevice device, size_t size, const void* p_code);
    ~ShaderData();

    VkShaderStageFlags shader_stage() const;
    VkShaderStageFlagBits shader_stage_bits() const;
    
    uint32_t max_descriptor_set() const;

    void append_layout_bindings(BindingsMap& bindings) const;

    void get_vertex_input_data(
        VkVertexInputBindingDescription* binding_desc,
        std::vector<VkVertexInputAttributeDescription>& attribute_description,
        VkPipelineVertexInputStateCreateInfo* ci) const;

    VkDevice device = VK_NULL_HANDLE;

    VkShaderModule shader_module = VK_NULL_HANDLE;
    SpvReflectShaderModule spv_module = {};
    std::vector<SpvReflectInterfaceVariable*> input_vars;
    std::vector<SpvReflectInterfaceVariable*> output_vars;
    std::vector<SpvReflectDescriptorBinding*> descriptor_bindings;
    std::vector<SpvReflectDescriptorSet*> descriptor_sets;
};