#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <random>
#include <array>
#include <functional>

#include <vulkan/vulkan.h>
#include <glm/vec2.hpp>

#include "imgui.h"

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

void chk(VkResult result);
void chk(bool result);
void chk_sdl(bool result);

template<class T>
std::vector<T> read_file_to_buffer(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "file: " << path << " does not exist" << std::endl;
        exit(-1);
    }

    std::ifstream source_file(path, std::ios::ate | std::ios::binary);
    if (!source_file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(-1);
    }

    const size_t file_size_bytes = (size_t) source_file.tellg();
    std::vector<T> buffer(file_size_bytes / sizeof(T));

    source_file.seekg(0);
    source_file.read((char*)buffer.data(), file_size_bytes);
    source_file.close();
    
    return buffer;
}

struct Radians;
struct Degrees {
    float val;

    Degrees operator+(const Degrees& rhs) const {
        return Degrees(val + rhs.val);
    }
    Degrees operator-(const Degrees& rhs) const {
        return Degrees(val - rhs.val);
    }

    Radians to_radians() const;
};

struct Radians {
    float val;

    Radians operator+(const Radians& rhs) const {
        return Radians(val + rhs.val);
    }
    Radians operator-(const Radians& rhs) const {
        return Radians(val - rhs.val);
    }

    Degrees to_degrees() const;
};

template<std::floating_point T>
T lerp(T a, T b, T f)
{
    return a * (1.0 - f) + (b * f);
}

inline glm::vec2 lerp_vec2(glm::vec2 a, glm::vec2 b, float f) {
    return glm::vec2(lerp(a.x, b.x, f), lerp(a.y, b.y, f));
}

template<std::floating_point T>
T random_num_in_range(T min, T max) {
    constexpr uint64_t rng_seed = 1234;
    static std::mt19937 rng(rng_seed);
    const T t = std::generate_canonical<T, 128>(rng);
    return lerp(min, max, t);
}

inline std::vector<std::string> get_filtered_file_strings(const std::string& path, std::function<bool(const std::filesystem::path&)> filter_fn, std::function<std::string(const std::filesystem::path&)> stringify_fn) {
    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        const std::filesystem::path entry_path = entry.path();
        if (std::filesystem::is_regular_file(entry) && filter_fn(entry_path)) {
            files.push_back(stringify_fn(entry_path));
        }
    }
    return files;
}

inline std::vector<std::string> get_file_paths_with_extension(const std::string& path, const std::string& extension) {
    return get_filtered_file_strings(
        path,
        [&](const std::filesystem::path& path) {
            return path.extension() == extension;
        },
        [](const std::filesystem::path& path) {
            return path.string();
        }
    );
}

inline std::vector<std::string> get_file_stems_with_extension(const std::string& path, const std::string& extension) {
    return get_filtered_file_strings(
        path,
        [&](const std::filesystem::path& path) {
            return path.extension() == extension;
        },
        [](const std::filesystem::path& path) {
            return path.stem().string();
        }
    );
}

namespace ImGui {
    inline bool ComboStringVec(const char* label, int* current_item, const std::vector<std::string>& items, int items_count, int height_in_items = -1)
    {
        return ImGui::Combo(
            label, current_item,
            [](void* data, int idx, const char** out_text) {
                std::vector<std::string>* vector = reinterpret_cast<std::vector<std::string>*>(data);
                if(idx < 0 || idx >= vector->size()) {
                    return false;
                }
                *out_text = vector->at(idx).c_str();
                return true;
            }, (void*)&items,
            items_count, height_in_items
        );
    }
};