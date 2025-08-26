#include "load_save.h"
#include "util.h"
#include "imgui.h"

LoadSave::LoadSave() 
    : has_unsaved_changes(false)
    , show_confirm_load_popup(false)
{
    memset(file_path, 0, MAX_FILE_PATH_SIZE);
}

void LoadSave::load_files(const char* dir, const char* extension) {
    files = get_file_stems_with_extension(dir, extension);
    std::vector<std::string>::iterator it = std::find(files.begin(), files.end(), std::string(file_path));
    if (it == files.end()) {
        selected_file = -1;
    } else {
        selected_file = it - files.begin();
    }
}

std::filesystem::path LoadSave::get_curr_file_path(const char* dir, const char* extension, const char* file_name) {
    std::filesystem::path dir_path(dir);
    std::filesystem::path file(file_name);
    std::filesystem::path out_path = dir_path / file;
    out_path.replace_extension(extension);
    return out_path;
}

void LoadSave::save(ImguiLog* logger, const char* dir, const char* extension, std::function<void(nlohmann::json&)> save_fn) {
    nlohmann::json root;
    save_fn(root);

    const std::filesystem::path curr_file_path = get_curr_file_path(dir, extension, file_path);
    std::ofstream out_file(curr_file_path);
    if (out_file.is_open()) {
        out_file << root;
        out_file.close();
        
        if (logger) {
            logger->add_log("saved file to: %s\n", curr_file_path.string());
        }
    } else {
        if (logger) {
            logger->add_log("ERROR saving file to: %s (%s)\n", curr_file_path.string(), strerror(errno));
        }
    }

    has_unsaved_changes = false;
    load_files(dir, extension);
}

void LoadSave::load(ImguiLog* logger, const char* dir, const char* extension, std::function<void(nlohmann::json&)> load_fn) {
    const std::string& path_str = files[selected_file];
    const std::filesystem::path curr_path = get_curr_file_path(dir, extension, path_str.c_str());
    std::ifstream in_file(curr_path);
    if (in_file.is_open()) {
        nlohmann::json root;
        in_file >> root;
        in_file.close();

        load_fn(root);

        if (logger) {
            logger->add_log("loaded file from: %s\n", curr_path.string());
        }
    } else {
        if (logger) {
            logger->add_log("ERROR loading file from: %s (%s)\n", curr_path.string(), strerror(errno));
        }
    }

    has_unsaved_changes = false;
}

LoadSave::ImguiResult LoadSave::imgui_node() {
    LoadSave::ImguiResult res = LoadSave::ImguiResult::Noop;
    if (show_confirm_load_popup) {
        ImGui::OpenPopup("Confirm Popup");

        if (ImGui::BeginPopup("Confirm Popup")) {
            ImGui::Text("You have unsaved changes. Confirm loading new particle?");

            if (ImGui::Button("Yes")) {
                ImGui::CloseCurrentPopup();

                show_confirm_load_popup = false;
                res = LoadSave::ImguiResult::ShouldLoad;
            }

            ImGui::SameLine();

            if (ImGui::Button("No")) {

                show_confirm_load_popup = false;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    ImGui::ComboStringVec("File Path##1", &selected_file, files, files.size());
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        if (has_unsaved_changes) {
            show_confirm_load_popup = true;
        } else {
            res = LoadSave::ImguiResult::ShouldLoad;
        }
    }

    ImGui::InputText("File Path##2", file_path, MAX_FILE_PATH_SIZE);
    ImGui::SameLine();
    if (ImGui::Button("Save")) {
        res = LoadSave::ImguiResult::ShouldSave;
    }

    return res;
}