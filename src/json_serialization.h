#pragma once

#include "nlohmann/json.hpp"

#include "glm/vec2.hpp"

inline void serialize_uint32(nlohmann::json& root, const char* field_name, uint32_t val) {
    root[field_name] = val;
}
inline uint32_t get_serialized_uint32(nlohmann::json& root, const char* field_name) {
    return root[field_name].get<uint32_t>();
}

inline void serialize_float(nlohmann::json& root, const char* field_name, float val) {
    root[field_name] = val;
}
inline float get_serialized_float(nlohmann::json& root, const char* field_name) {
    return root[field_name].get<float>();
}

inline void serialize_vec2(nlohmann::json& root, const char* field_name, glm::vec2 vec) {
    root[field_name] = {vec.x, vec.y};
}
inline glm::vec2 get_serialized_vec2(nlohmann::json& root, const char* field_name) {
    const std::array<float, 2> as_array = root[field_name].get<std::array<float, 2>>();
    return glm::vec2(as_array[0], as_array[1]);
}

inline void serialize_vec3(nlohmann::json& root, const char* field_name, glm::vec3 vec) {
    root[field_name] = {vec.x, vec.y, vec.z};
}
inline glm::vec3 get_serialized_vec3(nlohmann::json& root, const char* field_name) {
    const std::array<float, 3> as_array = root[field_name].get<std::array<float, 3>>();
    return glm::vec3(as_array[0], as_array[1], as_array[2]);
}