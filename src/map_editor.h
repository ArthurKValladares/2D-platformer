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
#include "load_save.h"

struct ImGuiTextureData {
    VkDescriptorSet DS;
};

struct MapEditor {
    MapEditor(Renderer* renderer);
    void cleanup(Renderer* renderer);

    void add_to_renderable(Renderer* renderer, Renderable& renderable);
    void update(const KeyboardState& keyboard_state, const MouseState& mouse_state, double total_elapsed_seconds, double frame_dt);
    void imgui_node(Renderer* renderer);

    void save(Renderer* renderer);
    void load(Renderer* renderer);

    void resize();

    uint32_t selected_tile_type;
    std::vector<ImGuiTextureData> my_textures;

    int width, height;
    std::vector<std::vector<TileType>> tiles;

    std::pair<int, int> selected_tile;

    OrthographicCamera camera;
    GlobalDescriptorSetData global_set_data;

    LoadSave load_save;
};