#include "descriptor_set.h"
#include "renderer.h"

DescriptorSetLayout::DescriptorSetLayout(const Renderer* renderer, std::span<const VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags) { 
    VkDescriptorSetLayoutCreateInfo ci = initializers::descriptor_set_create_info(bindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR);
    chk(vkCreateDescriptorSetLayout(renderer->get_device(), &ci, nullptr, &raw));
}

DescriptorSet::DescriptorSet(const Renderer* renderer, const DescriptorSetLayout& layout) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = renderer->get_descriptor_pool();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &layout.raw;
    chk(vkAllocateDescriptorSets(renderer->get_device(), &alloc_info, &raw));
}