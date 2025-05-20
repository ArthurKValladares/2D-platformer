#pragma once

#include <vector>
#include <utility>

#include "util.h"
#include "renderer/renderer.h"
#include "renderables/includes.h"
#include "keyboard_state.h"
#include "camera.h"
#include "global_descriptor_set.h"
#include "map_editor/tile_types.h"

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
    void update(const KeyboardState& keyboard_state, const MouseState& mouse_state, double total_elapsed_seconds, double frame_dt);
    void imgui_node(Renderer* renderer);

    void save(Renderer* renderer);
    void load(Renderer* renderer);

    // TODO: Duplicated from particles
    void load_map_files();
    std::filesystem::path get_curr_file_path(const char* file_name);

    void resize();

    uint32_t selected_tile_type;
    std::vector<MyTextureData> my_textures;

    int width, height;
    std::vector<std::vector<TileType>> tiles;

    std::pair<int, int> selected_tile;

    OrthographicCamera camera;
    GlobalDescriptorSetData global_set_data;

    // TODO: Abstract this logic together with particle stuff
    int selected_file;
    std::vector<std::string> map_files;

    static constexpr char map_dir[] = "./assets/maps";
    static constexpr char map_extension[] = ".json";

    static constexpr uint32_t MAX_FILE_PATH_SIZE = 256;
    char file_path[MAX_FILE_PATH_SIZE];

    bool has_unsaved_changes;
    bool show_confirm_load_popup;
};