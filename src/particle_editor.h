#pragma once

#include "particles.h"

#include <algorithm>

struct ParticleEditor {
    ParticleEditor()
        : emitter(ParticleEmitter(
            0.0,
            glm::vec2(0.0),
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
    
    void imgui_node() {
        ImGui::Text("Settings File");

        ImGui::ComboStringVec("File Path##1", &selected_file, particle_files, particle_files.size());
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            load();
        }

        ImGui::InputText("File Path##2", file_path, MAX_FILE_PATH_SIZE);
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            save();
        }

        emitter.imgui_node();
    }

    void save() {
        // TODO: need to re-load `particle_files` after saving
    }

    void load() {
        // TODO: Ask confirmation if I have unsaved changes
    }

    void load_particle_files() {
        particle_files = get_files_with_extension(particle_dir, particle_extension);
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
    static constexpr char default_file[] = ".assets/particles/default.ptcl";

    static constexpr uint32_t MAX_FILE_PATH_SIZE = 256;
    char file_path[MAX_FILE_PATH_SIZE];

    ParticleEmitter emitter;
};