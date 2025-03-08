#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

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
    std::vector<uint32_t> buffer(file_size_bytes / sizeof(T));

    source_file.seekg(0);
    source_file.read((char*)buffer.data(), file_size_bytes);
    source_file.close();
    
    return buffer;
}