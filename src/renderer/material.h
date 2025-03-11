#pragma once

#include <string.h>

#include "texture.h"

struct Renderer;
struct Material {
    Material() {}
    Material(Renderer* renderer, Texture* texture, VkDescriptorSetLayout set_layout);

    Texture* texture;
    VkDescriptorSet descriptor_set;
};