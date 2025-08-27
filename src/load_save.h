#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>

#include "json_serialization.h"
#include "logger.h"

#define MAP_DIR "./assets/maps"
#define MAP_EXTENSION ".json"

#define PARTICLES_DIR "./assets/particles"
#define PARTICLES_EXTENSION ".json"

struct LoadSave {
    LoadSave() {}

    void save(ImguiLog* logger, const char* dir, const char* extension, const char* file_name, std::function<void(nlohmann::json&)> save_fn);
    void load(ImguiLog* logger, const char* dir, const char* extension, const char* file_name, std::function<void(nlohmann::json&)> load_fn);
};

struct LoadSaveDir {
    LoadSaveDir();

    void load_files(const char* dir, const char* extension);

    void save(ImguiLog* logger, const char* dir, const char* extension, std::function<void(nlohmann::json&)> save_fn);
    void load(ImguiLog* logger, const char* dir, const char* extension, std::function<void(nlohmann::json&)> load_fn);

    enum class ImguiResult {
        Noop,
        ShouldSave,
        ShouldLoad
    };
    ImguiResult imgui_node();

    int selected_file;
    std::vector<std::string> files;

    static constexpr uint32_t MAX_FILE_PATH_SIZE = 256;
    char file_path[MAX_FILE_PATH_SIZE];

    bool has_unsaved_changes;
    bool show_confirm_load_popup;

    LoadSave load_save;
};