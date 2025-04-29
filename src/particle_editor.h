#pragma once

#include "particles.h"

#include <algorithm>

#include "nlohmann/json.hpp"

struct ParticleEditor {
    ParticleEditor()
        : emitter(ParticleEmitter(
            0.0,
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
    {
        memset(file_path, 0, MAX_FILE_PATH_SIZE);
        
        load_particle_files();
    }

    void add_to_renderable(Renderer* renderer, Renderable& renderable, double total_elapsed_seconds, BufferID global_data_buffer) {
        emitter.update_and_create_renderables(renderable, total_elapsed_seconds, renderer, global_data_buffer);
    }
    
    void imgui_node(double total_elapsed_seconds) {
        ImGui::Text("Settings File");

        ImGui::ComboStringVec("File Path##1", &selected_file, particle_files, particle_files.size());
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            load(total_elapsed_seconds);
        }

        ImGui::InputText("File Path##2", file_path, MAX_FILE_PATH_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            save();
        }

        emitter.imgui_node();
    }

private:
    std::filesystem::path get_curr_file_path() {
        std::filesystem::path particle_dir_path(particle_dir);
        std::filesystem::path file(file_path);
        std::filesystem::path out_path = particle_dir_path / file;
        out_path.replace_extension(particle_extension);
        return out_path;
    }

    void save() {
        // TODO: abstract this json logic
        nlohmann::json root;
        {
            const glm::vec2 center = emitter.center;
            root["center"] = {center.x, center.y};

            const glm::vec2 start_offset = emitter.start_offset.base_val;
            root["start_offset"] = {start_offset.x, start_offset.y};
            const glm::vec2 start_offset_var = emitter.start_offset.variability;
            root["start_offset_var"] = {start_offset_var.x, start_offset_var.y};

            root["emission_delay"] = emitter.emission_delay.base_val;
            root["emission_delay_var"] = emitter.emission_delay.variability;

            root["emission_angle"] = emitter.emission_angle.base_val.val;
            root["emission_angle_var"] = emitter.emission_angle.variability.val;

            root["particle_lifetime"] = emitter.particle_lifetime.base_val;
            root["particle_lifetime_var"] = emitter.particle_lifetime.variability;

            root["particle_vel"] = emitter.particle_vel.base_val;
            root["particle_vel_var"] = emitter.particle_vel.variability;

            const glm::vec2 start_size = emitter.size.start.base_val;
            root["start_size"] = {start_size.x, start_size.y};
            const glm::vec2 start_size_var = emitter.size.start.variability;
            root["start_size_var"] = {start_size_var.x, start_size_var.y};

            const glm::vec2 end_size = emitter.size.end.base_val;
            root["end_size"] = {end_size.x, end_size.y};
            const glm::vec2 end_size_var = emitter.size.end.variability;
            root["end_size_var"] = {end_size_var.x, end_size_var.y};

            const glm::vec3 start_color = emitter.color.start.base_val;
            root["start_color"] = {start_color.r, start_color.g, start_color.b};
            const glm::vec3 start_color_var = emitter.color.start.variability;
            root["start_color_var"] = {start_color_var.r, start_color_var.g, start_color_var.b};

            const glm::vec3 end_color = emitter.color.end.base_val;
            root["end_color"] = {end_color.r, end_color.g, end_color.b};
            const glm::vec3 end_color_var = emitter.color.end.variability;
            root["end_color_var"] = {end_color_var.r, end_color_var.g, end_color_var.b};
            
            root["texture"] = static_cast<uint32_t>(emitter.texture);
        }

        std::ofstream out_file(get_curr_file_path());
        if (out_file.is_open()) {
            out_file << root;
            out_file.close();
            // TODO: Log to an imgui `log` tab
        } else {
            // TODO: Log error to an imgui `log` tab
        }

        load_particle_files();
    }

    void load(double total_elapsed_seconds) {
        // TODO: Ask confirmation if I have unsaved changes

        std::ifstream in_file(get_curr_file_path());
        if (in_file.is_open()) {
            nlohmann::json root;
            in_file >> root;
            in_file.close();

            const double old_start_time = emitter.start_time;

            const std::array<float, 2> center = root["center"].get<std::array<float, 2>>();

            const std::array<float, 2> start_offset     = root["start_offset"].get<std::array<float, 2>>();
            const std::array<float, 2> start_offset_var = root["start_offset_var"].get<std::array<float, 2>>();

            const std::array<float, 2> start_size     = root["start_size"].get<std::array<float, 2>>();
            const std::array<float, 2> start_size_var = root["start_size_var"].get<std::array<float, 2>>();

            const std::array<float, 2> end_size     = root["end_size"].get<std::array<float, 2>>();
            const std::array<float, 2> end_size_var = root["end_size_var"].get<std::array<float, 2>>();

            const std::array<float, 3> start_color     = root["start_color"].get<std::array<float, 3>>();
            const std::array<float, 3> start_color_var = root["start_color_var"].get<std::array<float, 3>>();

            const std::array<float, 3> end_color     = root["end_color"].get<std::array<float, 3>>();
            const std::array<float, 3> end_color_var = root["end_color_var"].get<std::array<float, 3>>();

            emitter = ParticleEmitter(
                old_start_time,
                glm::vec2(center[0], center[1]),
                VariableField<glm::vec2>(
                    glm::vec2(start_offset[0],     start_offset[1]),
                    glm::vec2(start_offset_var[0], start_offset_var[1])
                ),
                VariableField<float>(
                    root["emission_delay"].get<float>(),
                    root["emission_delay_var"].get<float>()
                ),
                VariableField<Degrees>(
                    Degrees(root["emission_angle"].get<float>()),
                    Degrees(root["emission_angle_var"].get<float>())
                ),
                VariableField<float>(
                    root["particle_lifetime"].get<float>(),
                    root["particle_lifetime_var"].get<float>()
                ),
                VariableField<float>(
                    root["particle_vel"].get<float>(),
                    root["particle_vel_var"].get<float>()
                ),
                InterpolatableField<VariableField<glm::vec2>>(
                    VariableField<glm::vec2>(
                        glm::vec2(start_size[0],     start_size[1]),
                        glm::vec2(start_size_var[0], start_size_var[1])
                    ),
                    VariableField<glm::vec2>(
                        glm::vec2(end_size[0],     end_size[1]),
                        glm::vec2(end_size_var[0], end_size_var[1])
                    )
                ),
                InterpolatableField<VariableField<glm::vec3>>(
                    VariableField<glm::vec3>(
                        glm::vec3(start_color[0],     start_color[1],     start_color[2]),
                        glm::vec3(start_color_var[0], start_color_var[1], start_color_var[2])
                    ),
                    VariableField<glm::vec3>(
                        glm::vec3(end_color[0],     end_color[1],     end_color[2]),
                        glm::vec3(end_color_var[0], end_color_var[1], end_color_var[2])
                    )
                ),
                static_cast<TextureSource>(root["texture"].get<uint32_t>())
            );

            // TODO: Log to an imgui `log` tab
        } else {
            // TODO: Log error to an imgui `log` tab
        }
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

    ParticleEmitter emitter;

};