#pragma once

#include <string.h>

#include "texture.h"

struct Renderer;
struct Material {
    Material() {}
    // TODO: Still kinda bad
    Material(Renderer* renderer, const Texture* texture, VkDescriptorSetLayout set_layout, uint32_t binding);

    Texture* texture;
    VkDescriptorSet descriptor_set;
};