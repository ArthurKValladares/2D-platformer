#pragma once

#include "particles.h"
#include "particle_loader.h"
#include "keyboard_state.h"
#include "camera.h"
#include "global_descriptor_set.h"
#include "renderer/renderer.h"
#include "view.h"

#include <algorithm>

struct ParticleEditor final : View {
    ParticleEditor(const Window& window, Renderer* renderer)
        : emitter(ParticleEmitter(
            0.0,
            EmitterLifetime::infinite(),
            glm::vec2(0.0),
            VariableField<glm::vec2>(glm::vec2(0.0, 0.0)),
            VariableField<float>(0.0),
            VariableField<Degrees>(Degrees(90.0), Degrees(15.0)),
            VariableField<float>(2.0, 0.5),
            VariableField<float>(1.0, 0.25),
            InterpolatableField<VariableField<glm::vec2>>(
                VariableField<glm::vec2>(glm::vec2(0.3, 0.3), glm::vec2(.125)),
                glm::vec2(0.0, 0.0)
            ),
            InterpolatableField<VariableField<glm::vec3>>(
                VariableField<glm::vec3>(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.05, 0.0, 0.0)),
                VariableField<glm::vec3>(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.05))
            ),
            TextureSource::Particle
        ))
        , global_set_data(GlobalDescriptorSetData(renderer, camera))
    {
        const Size2Di32 window_size = window.get_size();
        const float scale = static_cast<float>(emitter.size.start.base_val.x * DEFAULT_SIZE_SCALE) / window_size.width;
        camera = OrthographicCamera(
            glm::vec2(0.0),
            glm::vec2(window_size.width, window_size.height),
            scale
        );

        global_set_data.write_shader_data_to_buffer(renderer);
        update_global_set(renderer, global_set_data.buffer_id, global_set_data.set_id);

        
        load_save.load_files(PARTICLES_DIR, PARTICLES_EXTENSION);
    }

    RootRenderable draw_fn(Renderer* renderer, double total_elapsed_seconds) {
        global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
        global_set_data.write_shader_data_to_buffer(renderer);

        Renderable renderable = emitter.draw(total_elapsed_seconds);

        return RootRenderable {
            renderable,
            global_set_data.layout_id,
            global_set_data.set_id
        };
    }

    bool should_draw_ui() const {
        return false;
    }
    
    RootRenderable draw_ui(UI& ui, Renderer* renderer, double total_elapsed_time) {
        Renderable renderable;

        return RootRenderable {
            renderable,
            ui.global_descriptor_set.layout_id,
            ui.global_descriptor_set.set_id
        };
    }
    
    const char* name() const {
        return "Particle Editor";
    }

    void update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt) {
        camera.update(context.keyboard_state, frame_dt, total_elapsed_seconds);
        emitter.update(total_elapsed_seconds);
    }

    void draw_imgui(ImguiLog& logger, double total_elapsed_seconds) {
        ImGui::Text("Settings File");

        LoadSaveDir::ImguiResult res = load_save.imgui_node();
        if (res == LoadSaveDir::ImguiResult::ShouldSave) {
            save(logger);
        } else if (res == LoadSaveDir::ImguiResult::ShouldLoad) {
            load(logger, total_elapsed_seconds);
        }
        
        const bool has_changed = emitter.imgui_node();
        if (has_changed) {
            load_save.has_unsaved_changes = true;
        }
    }

    void cleanup(Renderer* renderer) {}
private:
    void save(ImguiLog& logger) {
        load_save.save(&logger, PARTICLES_DIR, PARTICLES_EXTENSION, [&](nlohmann::json& root) {
            save_particle_fn(emitter, root);
        });
    }

    void load(ImguiLog& logger, double total_elapsed_seconds) {
        load_save.load(&logger, PARTICLES_DIR, PARTICLES_EXTENSION, [&](nlohmann::json& root) {
            load_particle_fn(emitter, root, total_elapsed_seconds);
        });
    }

    ParticleEmitter emitter;

    static constexpr float DEFAULT_SIZE_SCALE = 10.0;

    LoadSaveDir load_save;
public:
    OrthographicCamera camera;
    GlobalDescriptorSetData global_set_data;
};