#pragma once

#include <vector>

#include <vulkan/vulkan.h>
#include "spirv_reflect.h"

struct ShaderData {
    ShaderData(VkDevice device, size_t size, const void* p_code);
    ~ShaderData();

    VkShaderStageFlags shader_stage() const;
    VkShaderStageFlagBits shader_stage_bits() const;
    
    std::vector<VkDescriptorSetLayoutBinding> get_layout_bindings(uint32_t set_idx) const;

    VkDevice device;

    VkShaderModule shader_module;
    SpvReflectShaderModule spv_module;
    std::vector<SpvReflectInterfaceVariable*> input_vars;
    std::vector<SpvReflectInterfaceVariable*> output_vars;
    std::vector<SpvReflectDescriptorBinding*> descriptor_bindings;
    std::vector<SpvReflectDescriptorSet*> descriptor_sets;
};