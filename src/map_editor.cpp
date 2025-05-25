#include "map_editor.h"

#include "renderer/renderer.h"
#include "assets.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "stb_image.h"
#include "json_serialization.h"
#include "image.h"

#define DEFAULT_WIDTH  15
#define DEFAULT_HEIGHT 15
#define MIN_WIDTH      3
#define MIN_HEIGHT     3

namespace {
void RemoveTexture(Renderer* renderer, ImGuiTextureData* tex_data)
{
    ImGui_ImplVulkan_RemoveTexture(tex_data->DS);
}
};

MapEditor::MapEditor(Renderer* renderer)
    : width(DEFAULT_WIDTH)
    , height(DEFAULT_HEIGHT)
    , camera(OrthographicCamera(
        glm::vec2(0.0),
        DEFAULT_WIDTH * TILE_SIZE,
        DEFAULT_WIDTH * TILE_SIZE
    ))
    , global_set_data(GlobalDescriptorSetData(renderer, camera))
 {
    resize();

    global_set_data.write_shader_data_to_buffer(renderer);
    update_global_set(renderer, global_set_data.buffer_id, global_set_data.set_id);
    
    selected_tile_type = 0;
    const uint32_t num_tile_types = tile_type_count();
    my_textures.resize(num_tile_types);
    for (uint32_t i = 0; i < num_tile_types; ++i) {
        const TileType tile_ty = tile_type_from_uint(i);
        const TextureSource texture_src = tile_type_to_texture(tile_ty);
        const TextureID tex_id = texture_id(texture_src);

        if (!renderer->contains_texture(tex_id)) {
            const ImageData image_data = ImageData(texture_path(texture_src));
            renderer->upload_texture(tex_id, image_data.texture_create_info());
        }

        const Texture& texture = renderer->get_texture(texture_id(texture_src));

        my_textures[i].DS = ImGui_ImplVulkan_AddTexture(texture.sampler, texture.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    selected_tile = std::make_pair(-1, -1);

    load_save.load_files(MAP_DIR, MAP_EXTENSION);
}

void MapEditor::cleanup(Renderer* renderer) {
    // TODO: Pretty bad so far since its not integrated with the Renderer
    vkDeviceWaitIdle(renderer->get_device());

    for (ImGuiTextureData& my_texture : my_textures) {
        RemoveTexture(renderer, &my_texture);
    }
}

void MapEditor::add_to_renderable(Renderer* renderer, Renderable& renderable) {
    global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
    global_set_data.write_shader_data_to_buffer(renderer);

    const Rect2D camera_rect = camera.get_rect();

    for (uint64_t row = 0; row < height; ++row) {
        for (uint64_t col = 0; col < width; ++col) {
            const Rect2D rect = Rect2D(glm::vec2(col * TILE_SIZE, row * TILE_SIZE), glm::vec2(TILE_SIZE, TILE_SIZE));

            if (rect.intersects(camera_rect)) {
                const TileType ty = tiles[row][col];

                const bool is_selected = selected_tile == std::make_pair(row, col);
                const float outline_thickness = is_selected
                    ? 0.03
                    : 0.01;

                const glm::vec3 outline_color = is_selected
                    ? glm::vec3(0.0, 0.0, 1.0)
                    : glm::vec3(1.0, 0.0, 0.0);

                renderable.push_child(OutlineQuad(
                    renderer,
                    rect,
                    tile_type_to_texture(ty),
                    OutlinePushConstantData{
                        .color = tile_type_to_color(ty),
                        .outline = outline_color,
                        .thickness = outline_thickness
                    },
                    global_set_data.buffer_id
                ));
            }
        }
    }
}

void MapEditor::update(const KeyboardState& keyboard_state, const MouseState& mouse_state, double total_elapsed_seconds, double frame_dt) {
    // TODO THis pattern is repeated a lot
    constexpr float displacement_per_second = 5.0;
    glm::vec2 movement_vec{0.0, 0.0};
    if (keyboard_state.is_down(SDLK_A)) {
        movement_vec.x -= 1.0;
    }
    if (keyboard_state.is_down(SDLK_W)){
        movement_vec.y += 1.0;
    }
    if (keyboard_state.is_down(SDLK_S)){
        movement_vec.y -= 1.0;
    }
    if (keyboard_state.is_down(SDLK_D)){
        movement_vec.x += 1.0;
    }

    if (glm::length(movement_vec) > 0.0) {
        movement_vec = glm::normalize(movement_vec);

        const float displacement = displacement_per_second * frame_dt;
        const glm::vec2 displacement_vec = movement_vec * displacement;

        camera.center += displacement_vec;
    }

    constexpr float camera_zoom_vel = 0.5;
    if (keyboard_state.is_down(SDLK_E)) {
        camera.scale += camera_zoom_vel * frame_dt;
    }
    if (keyboard_state.is_down(SDLK_Q)) {
        camera.scale -= camera_zoom_vel * frame_dt;
        camera.scale = std::max(0.1f, camera.scale);
    }

    // Find selected tile
    selected_tile = std::make_pair(-1, -1);
    const glm::vec2 mouse_pos = camera.center + (mouse_state.world_space_pos() * glm::vec2(camera.size_x / 2.0, -camera.size_y / 2.0));

    for (uint32_t row = 0; row < height; ++row) {
        for (uint32_t col = 0; col < width; ++col) {
            const Rect2D rect = Rect2D(glm::vec2(col * TILE_SIZE, row * TILE_SIZE), glm::vec2(TILE_SIZE, TILE_SIZE));
            if (rect.intersects_point(mouse_pos)) {
                selected_tile = std::make_pair(row, col);
                if (mouse_state.is_down(SDL_BUTTON_LEFT)) {
                    tiles[row][col] = tile_type_from_uint(selected_tile_type);
                    load_save.set_has_unsaved_changes(true);
                }
            }
        }
    }
}

void MapEditor::resize() {
    tiles.resize(height);
    for (std::vector<TileType>& row : tiles) {
        row.resize(width);
    }
}

void MapEditor::imgui_node(Renderer* renderer) {
    constexpr uint32_t textures_per_line = 4;

    LoadSave::ImguiResult res = load_save.imgui_node();
    if (res == LoadSave::ImguiResult::ShouldSave) {
        save(renderer);
    } else if (res == LoadSave::ImguiResult::ShouldLoad) {
        load(renderer);
    }

    bool has_changed = false;
    if (ImGui::InputInt("Width", &width) && width < MIN_WIDTH) {
        has_changed = true;
        width = MIN_WIDTH;
    }
    if (ImGui::InputInt("Height", &height) && height < MIN_HEIGHT) {
        has_changed = true;
        height = MIN_HEIGHT;
    }
    resize();

    const ImVec2 size = ImVec2(128.0f, 128.0f);
    for (uint32_t id = 0; id < my_textures.size(); ++id) {
        bool is_selected_type = id == selected_tile_type;
        if (is_selected_type) {
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0, 0, 0, 1.0));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, ImGui::GetIO().FontGlobalScale * 2.0);
        }

        const ImGuiTextureData& my_texture = my_textures[id];
        
        if (id % textures_per_line != 0) {
            ImGui::SameLine();
        }

        ImGui::PushID(id);
        if (ImGui::ImageButton("##", (ImTextureID) my_texture.DS, size)) {
            selected_tile_type = id;
        }
        ImGui::PopID();

        if (is_selected_type) {
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
    }

    if (has_changed) {
        load_save.set_has_unsaved_changes(true);
    }
}

void MapEditor::save(Renderer* renderer) {
    load_save.save(MAP_DIR, MAP_EXTENSION, renderer, [&](nlohmann::json& root) {
        serialize_uint32(root, "width", tiles[0].size());
        serialize_uint32(root, "height", tiles.size());
        std::vector<std::vector<uint32_t>> tiles_uint;
        for (const std::vector<TileType>& row : tiles) {
            std::vector<uint32_t> row_uint;
            for (TileType ty : row) {
                row_uint.push_back(static_cast<uint32_t>(ty));
            }
            tiles_uint.push_back(std::move(row_uint));
        }
        serialize_vector(root, "tiles", tiles_uint);
    });
}

void MapEditor::load(Renderer* renderer) {
    load_save.load(MAP_DIR, MAP_EXTENSION, renderer, [&](nlohmann::json& root) {
        width = get_serialized_uint32(root, "width");
        height = get_serialized_uint32(root, "height");
        const std::vector<std::vector<uint32_t>> tiles = get_serialized_vector<std::vector<uint32_t>>(root, "tiles");
    });
}