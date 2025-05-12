#pragma once

#include "util.h"
#include "renderer/renderer.h"
#include "renderables/includes.h"

// TODO: see note in cpp
struct MyTextureData
{
    VkDescriptorSet DS;
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

struct MapEditor {
    MapEditor(Renderer* renderer);
    void cleanup(Renderer* renderer);

    void add_to_renderable(Renderer* renderer, Renderable& renderable, Rect2D camera_rect, BufferID global_data_buffer);

    void imgui_node(Renderer* renderer);

    int width, height;
    std::vector<MyTextureData> my_textures;
};