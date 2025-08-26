#include "map_editor.h"

#include "renderer/renderer.h"
#include "assets.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "stb_image.h"
#include "json_serialization.h"
#include "image.h"
#include "helpers.h"

#define DEFAULT_WIDTH  15
#define DEFAULT_HEIGHT 15
#define MIN_WIDTH      3
#define MIN_HEIGHT     3

namespace {
void RemoveTexture(ImGuiTextureData* tex_data)
{
    ImGui_ImplVulkan_RemoveTexture(tex_data->DS);
}
};

MapEditor::MapEditor(const Window& window, Renderer* renderer)
    : width(DEFAULT_WIDTH)
    , height(DEFAULT_HEIGHT)
    , global_set_data(GlobalDescriptorSetData(renderer, camera))
 {
    const Size2Di32 window_size = window.get_size();
    const float scale = static_cast<float>(DEFAULT_WIDTH * TILE_SIZE) / window_size.width;
    camera = OrthographicCamera(
        glm::vec2(0.0),
        glm::vec2(window_size.width, window_size.height),
        scale,
        0.0
    );

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
        RemoveTexture(&my_texture);
    }
}

RootRenderable MapEditor::draw_fn(Renderer* renderer, double total_elapsed_time) {
    Renderable renderable;
    
    global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
    global_set_data.write_shader_data_to_buffer(renderer);

    const Rect2D camera_rect = camera.rect();

    for (uint32_t row = 0; row < height; ++row) {
        for (uint32_t col = 0; col < width; ++col) {
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

                renderable.push_child(outline_quad(
                    rect,
                    texture_id(tile_type_to_texture(ty)),
                    OutlinePushConstantData{
                        .color = tile_type_to_color(ty),
                        .outline = outline_color,
                        .thickness = outline_thickness
                    }
                ));
            }
        }
    }

    return RootRenderable {
        renderable,
        global_set_data.layout_id,
        global_set_data.set_id
    };
}

RootRenderable MapEditor::draw_ui(Renderer* renderer, double total_elapsed_time) {
    Renderable renderable;

    return RootRenderable {
        renderable,
        global_set_data.layout_id,
        global_set_data.set_id
    };
}

void MapEditor::update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt) {
    constexpr float displacement_per_second = 5.0;
    const glm::vec2 displacement_vec = context.keyboard_state.displacement_vector(displacement_per_second, frame_dt, SDLK_A, SDLK_D, SDLK_W, SDLK_S);
    if (glm::length(displacement_vec) != 0.0) {
        camera.mark_move_to(camera.center + displacement_vec, total_elapsed_seconds);
    }
    
    camera.update(context.keyboard_state, frame_dt, total_elapsed_seconds);

    // Find selected tile
    selected_tile = std::make_pair(-1, -1);

    const glm::vec2 mouse_pos = get_screen_pos(context.window, context.mouse_state, camera);

    for (uint32_t row = 0; row < height; ++row) {
        for (uint32_t col = 0; col < width; ++col) {
            const Rect2D rect = Rect2D(glm::vec2(col * TILE_SIZE, row * TILE_SIZE), glm::vec2(TILE_SIZE, TILE_SIZE));
            if (rect.intersects_point(mouse_pos)) {
                selected_tile = std::make_pair(row, col);
                if (context.mouse_state.is_down(SDL_BUTTON_LEFT)) {
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

void MapEditor::draw_imgui(ImguiLog& logger, double total_elapsed_time) {
    constexpr uint32_t textures_per_line = 4;

    LoadSave::ImguiResult res = load_save.imgui_node();
    if (res == LoadSave::ImguiResult::ShouldSave) {
        save(logger);
    } else if (res == LoadSave::ImguiResult::ShouldLoad) {
        load(logger);
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

void MapEditor::save(ImguiLog& logger) {
    load_save.save(&logger, MAP_DIR, MAP_EXTENSION, [&](nlohmann::json& root) {
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

void MapEditor::load(ImguiLog& logger) {
    load_save.load(&logger, MAP_DIR, MAP_EXTENSION, [&](nlohmann::json& root) {
        width = get_serialized_uint32(root, "width");
        height = get_serialized_uint32(root, "height");
        const std::vector<std::vector<uint32_t>> tiles = get_serialized_vector<std::vector<uint32_t>>(root, "tiles");
    });
}