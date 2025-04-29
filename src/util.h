#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <random>
#include <array>

#include <vulkan/vulkan.h>

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

template<std::floating_point T>
T random_num_in_range(T min, T max) {
    constexpr uint64_t rng_seed = 1234;
    static std::mt19937 rng(rng_seed);
    const T t = std::generate_canonical<T, 128>(rng);
    return lerp(min, max, t);
}