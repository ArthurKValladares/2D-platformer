#pragma once

#include "util.h"

// TODO: see note in cpp
// A struct to manage data related to one image in vulkan
struct MyTextureData
{
    VkDescriptorSet DS;         // Descriptor set: this is what you'll pass to Image()
    int             Width;
    int             Height;
    int             Channels;

    // Need to keep track of these to properly cleanup
    VkImageView     ImageView;
    VkImage         Image;
    VkDeviceMemory  ImageMemory;
    VkSampler       Sampler;
    VkBuffer        UploadBuffer;
    VkDeviceMemory  UploadBufferMemory;

    MyTextureData() { memset(this, 0, sizeof(*this)); }
};

struct Renderer;
struct MapEditor {
    MapEditor(Renderer* renderer);
    void cleanup(Renderer* renderer);

    void imgui_node(Renderer* renderer);

    std::vector<MyTextureData> my_textures;
};