#pragma once

#include "util.h"
#include "renderer/renderer.h"
#include "renderables/includes.h"
#include "keyboard_state.h"
#include "camera.h"
#include "global_descriptor_set.h"

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

    void add_to_renderable(Renderer* renderer, Renderable& renderable);
    void update(const KeyboardState& keyboard_state, double total_elapsed_seconds, double frame_dt);
    void imgui_node(Renderer* renderer);

    int width, height;
    std::vector<MyTextureData> my_textures;

    OrthographicCamera camera;
    GlobalDescriptorSetData global_set_data;
};