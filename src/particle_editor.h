#pragma once

#include "particles.h"
#include "json_serialization.h"
#include "keyboard_state.h"
#include "camera.h"
#include "global_descriptor_set.h"
#include "renderer/renderer.h"
#include "load_save.h"
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
        Renderable renderable;

        global_set_data.shader_data.proj_matrix = camera.get_proj_matrix();
        global_set_data.write_shader_data_to_buffer(renderer);

        emitter.update_and_create_renderables(&renderable, total_elapsed_seconds);

        return RootRenderable {
            renderable,
            global_set_data.layout_id,
            global_set_data.set_id
        };
    }

    bool should_draw_ui() const {
        return false;
    }
    
    RootRenderable draw_ui(Renderer* renderer, double total_elapsed_time) {
        Renderable renderable;

        return RootRenderable {
            renderable,
            global_set_data.layout_id,
            global_set_data.set_id
        };
    }
    
    const char* name() const {
        return "Particle Editor";
    }

    void update_fn(const Window& window, const KeyboardState& keyboard_state, const MouseState& _mouse_state, double total_elapsed_seconds, double frame_dt) {
        camera.update(keyboard_state, frame_dt, total_elapsed_seconds);
    }

    void draw_imgui(ImguiLog& logger, double total_elapsed_seconds) {
        ImGui::Text("Settings File");

        LoadSave::ImguiResult res = load_save.imgui_node();
        if (res == LoadSave::ImguiResult::ShouldSave) {
            save(logger);
        } else if (res == LoadSave::ImguiResult::ShouldLoad) {
            load(logger, total_elapsed_seconds);
        }
        
        const bool has_changed = emitter.imgui_node();
        if (has_changed) {
            load_save.set_has_unsaved_changes(true);
        }
    }

    void cleanup(Renderer* renderer) {}
private:
    void save(ImguiLog& logger) {
        load_save.save(logger, PARTICLES_DIR, PARTICLES_EXTENSION, [&](nlohmann::json& root) {
            serialize_float(root, "lifetime", emitter.lifetime.val);
            serialize_vec2(root, "center", emitter.center);
            serialize_vec2(root, "start_offset", emitter.start_offset.base_val);
            serialize_vec2(root, "start_offset_var", emitter.start_offset.variability);
            serialize_float(root, "emission_delay", emitter.emission_delay.base_val);
            serialize_float(root, "emission_delay_var", emitter.emission_delay.variability);
            serialize_float(root, "emission_angle", emitter.emission_angle.base_val.val);
            serialize_float(root, "emission_angle_var", emitter.emission_angle.variability.val);
            serialize_float(root, "particle_lifetime", emitter.particle_lifetime.base_val);
            serialize_float(root, "particle_lifetime_var", emitter.particle_lifetime.variability);
            serialize_float(root, "particle_vel", emitter.particle_vel.base_val);
            serialize_float(root, "particle_vel_var", emitter.particle_vel.variability);
            serialize_vec2(root, "start_size", emitter.size.start.base_val);
            serialize_vec2(root, "start_size_var", emitter.size.start.variability);
            serialize_vec2(root, "end_size", emitter.size.end.base_val);
            serialize_vec2(root, "end_size_var", emitter.size.end.variability);
            serialize_vec3(root, "start_color", emitter.color.start.base_val);
            serialize_vec3(root, "start_color_var", emitter.color.start.variability);
            serialize_vec3(root, "end_color", emitter.color.end.base_val);
            serialize_vec3(root, "end_color_var", emitter.color.end.variability);
            serialize_uint32(root, "texture", static_cast<uint32_t>(emitter.texture));
        });
    }

    void load(ImguiLog& logger, double total_elapsed_seconds) {
        load_save.load(logger, PARTICLES_DIR, PARTICLES_EXTENSION, [&](nlohmann::json& root) {
            const double old_start_time = emitter.start_time;

            emitter = ParticleEmitter(
                old_start_time,
                EmitterLifetime(get_serialized_float(root, "lifetime")),
                get_serialized_vec2(root, "center"),
                VariableField<glm::vec2>(
                    get_serialized_vec2(root, "start_offset"),
                    get_serialized_vec2(root, "start_offset_var")
                ),
                VariableField<float>(
                    get_serialized_float(root, "emission_delay"),
                    get_serialized_float(root, "emission_delay_var")
                ),
                VariableField<Degrees>(
                    Degrees(get_serialized_float(root, "emission_angle")),
                    Degrees(get_serialized_float(root, "emission_angle_var"))
                ),
                VariableField<float>(
                    get_serialized_float(root, "particle_lifetime"),
                    get_serialized_float(root, "particle_lifetime_var")
                ),
                VariableField<float>(
                    get_serialized_float(root, "particle_vel"),
                    get_serialized_float(root, "particle_vel_var")
                ),
                InterpolatableField<VariableField<glm::vec2>>(
                    VariableField<glm::vec2>(
                        get_serialized_vec2(root, "start_size"),
                        get_serialized_vec2(root, "start_size_var")
                    ),
                    VariableField<glm::vec2>(
                        get_serialized_vec2(root, "end_size"),
                        get_serialized_vec2(root, "end_size_var")
                    )
                ),
                InterpolatableField<VariableField<glm::vec3>>(
                    VariableField<glm::vec3>(
                        get_serialized_vec3(root, "start_color"),
                        get_serialized_vec3(root, "start_color_var")
                    ),
                    VariableField<glm::vec3>(
                        get_serialized_vec3(root, "end_color"),
                        get_serialized_vec3(root, "end_color_var")
                    )
                ),
                static_cast<TextureSource>(get_serialized_uint32(root, "texture"))
            );
        });
    }

    ParticleEmitter emitter;

    static constexpr float DEFAULT_SIZE_SCALE = 10.0;

    LoadSave load_save;
public:
    OrthographicCamera camera;
    GlobalDescriptorSetData global_set_data;
};