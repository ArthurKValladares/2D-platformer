#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>

#include "json_serialization.h"

#define MAP_DIR "./assets/maps"
#define MAP_EXTENSION ".json"

#define PARTICLES_DIR "./assets/particles"
#define PARTICLES_EXTENSION ".json"

struct Renderer;
struct LoadSave {
    LoadSave();

    void load_files(const char* dir, const char* extension);
    std::filesystem::path get_curr_file_path(const char* dir, const char* extension, const char* file_name);

    void set_has_unsaved_changes(bool val) {
        has_unsaved_changes = val;
    }
    
    enum class ImguiResult {
        Noop,
        ShouldSave,
        ShouldLoad
    };
    ImguiResult imgui_node();

    void save(const char* dir, const char* extension, Renderer* renderer, std::function<void(nlohmann::json&)> save_fn);
    void load(const char* dir, const char* extension, Renderer* renderer, std::function<void(nlohmann::json&)> load_fn);

    int selected_file;
    std::vector<std::string> files;

    static constexpr uint32_t MAX_FILE_PATH_SIZE = 256;
    char file_path[MAX_FILE_PATH_SIZE];

    bool has_unsaved_changes;
    bool show_confirm_load_popup;
};