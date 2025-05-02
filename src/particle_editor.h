#pragma once

#include "particles.h"
#include "json_serialization.h"

#include <algorithm>

#include "nlohmann/json.hpp"

struct ParticleEditor {
    ParticleEditor()
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
        , has_unsaved_changes(false)
        , show_confirm_load_popup(false)
    {
        memset(file_path, 0, MAX_FILE_PATH_SIZE);
        
        load_particle_files();
    }

    void add_to_renderable(Renderer* renderer, Renderable& renderable, double total_elapsed_seconds, BufferID global_data_buffer) {
        emitter.update_and_create_renderables(renderable, total_elapsed_seconds, renderer, global_data_buffer);
    }
    
    void imgui_node(Renderer* renderer, double total_elapsed_seconds) {
        ImGui::Text("Settings File");

        if (show_confirm_load_popup) {
            ImGui::OpenPopup("Confirm Popup");

            if (ImGui::BeginPopup("Confirm Popup")) {
                ImGui::Text("You have unsaved changes. Confirm loading new particle?");

                if (ImGui::Button("Yes")) {
                    ImGui::CloseCurrentPopup();

                    show_confirm_load_popup = false;
                    load(renderer, total_elapsed_seconds);
                }

                ImGui::SameLine();

                if (ImGui::Button("No")) {

                    show_confirm_load_popup = false;
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::ComboStringVec("File Path##1", &selected_file, particle_files, particle_files.size());
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            if (has_unsaved_changes) {
                show_confirm_load_popup = true;
            } else {
                load(renderer, total_elapsed_seconds);
            }
        }

        ImGui::InputText("File Path##2", file_path, MAX_FILE_PATH_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            save(renderer);
        }
        
        const bool has_changed = emitter.imgui_node();
        has_unsaved_changes |= has_changed;
    }

private:
    std::filesystem::path get_curr_file_path(const char* file_name) {
        std::filesystem::path particle_dir_path(particle_dir);
        std::filesystem::path file(file_name);
        std::filesystem::path out_path = particle_dir_path / file;
        out_path.replace_extension(particle_extension);
        return out_path;
    }

    void save(Renderer* renderer) {
        // TODO: abstract this json logic
        nlohmann::json root;
        {
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
        }

        std::ofstream out_file(get_curr_file_path(file_path));
        if (out_file.is_open()) {
            out_file << root;
            out_file.close();
            
            renderer->logger().add_log("saved particle effect to: %s/%s%s\n", particle_dir, file_path, particle_extension);
        } else {
            renderer->logger().add_log("ERROR saving particle effect to: %s/%s%s (%s)\n", particle_dir, file_path, particle_extension, strerror(errno));
        }

        has_unsaved_changes = false;
        load_particle_files();
    }

    void load(Renderer* renderer, double total_elapsed_seconds) {
        std::ifstream in_file(get_curr_file_path(particle_files[selected_file].c_str()));
        if (in_file.is_open()) {
            nlohmann::json root;
            in_file >> root;
            in_file.close();

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

            renderer->logger().add_log("loaded particle effect from: %s/%s%s\n", particle_dir, file_path, particle_extension);
        } else {
            renderer->logger().add_log("ERROR loading particle effect from: %s/%s%s (%s)\n", particle_dir, file_path, particle_extension, strerror(errno));
        }

        has_unsaved_changes = false;
    }

    void load_particle_files() {
        particle_files = get_file_stems_with_extension(particle_dir, particle_extension);
        std::vector<std::string>::iterator it = std::find(particle_files.begin(), particle_files.end(), std::string(file_path));
        if (it == particle_files.end()) {
            selected_file = -1;
        } else {
            selected_file = it - particle_files.begin();
        }
    }

    int selected_file;
    std::vector<std::string> particle_files;

    static constexpr char particle_dir[] = "./assets/particles";
    static constexpr char particle_extension[] = ".ptcl";

    static constexpr uint32_t MAX_FILE_PATH_SIZE = 256;
    char file_path[MAX_FILE_PATH_SIZE];

    bool has_unsaved_changes;
    bool show_confirm_load_popup;

    ParticleEmitter emitter;

};