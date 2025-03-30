#pragma once

#include <span>

#include <vulkan/vulkan.h>

#include "initializers.h"
#include "../util.h"

struct Renderer;
struct DescriptorSetLayout {
    DescriptorSetLayout() {}
    DescriptorSetLayout(const Renderer* renderer, std::span<const VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags = {});

    VkDescriptorSetLayout raw = VK_NULL_HANDLE;
};

struct DescriptorSet {
    DescriptorSet() {}
    DescriptorSet(const Renderer* renderer, const DescriptorSetLayout& layout);

    VkDescriptorSet raw;
};