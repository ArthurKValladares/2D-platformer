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
#include "view.h"

struct ImGuiTextureData {
    VkDescriptorSet DS;
};

struct MapEditor final : View {
    MapEditor(Renderer* renderer);


    void cleanup(Renderer* renderer);

    const char* name() const {
        return "Map Editor";
    }
    void update_fn(const KeyboardState& keyboard_state, const MouseState& mouse_state, double total_elapsed_seconds, double frame_dt);
    ViewDrawData draw_fn(Renderer* renderer, Renderable* renderable, double total_elapsed_time);
    void draw_imgui(ImguiLog& logger, double total_elapsed_time);

    void save(ImguiLog& logger);
    void load(ImguiLog& logger);

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