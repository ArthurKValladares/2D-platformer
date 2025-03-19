#include "material.h"
#include "renderer.h"

Material::Material(Renderer* renderer, const Texture* texture, VkDescriptorSetLayout set_layout, uint32_t binding) {
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = renderer->get_descriptor_pool();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &set_layout;
    chk(vkAllocateDescriptorSets(renderer->get_device(), &alloc_info, &descriptor_set));

    // Update set
    VkWriteDescriptorSet descriptor_writes[1] = {
        VkWriteDescriptorSet {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = descriptor_set,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &texture->descriptor
        }
    };
    vkUpdateDescriptorSets(renderer->get_device(), ArrayCount(descriptor_writes), &descriptor_writes[0], 0, nullptr);
}