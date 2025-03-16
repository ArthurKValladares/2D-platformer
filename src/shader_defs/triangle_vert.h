#pragma once

#include <glm/vec3.hpp>
#include <vector>

#include "../renderer/draw.h"

struct TriangleVertex {
    glm::vec3 in_position;
    glm::vec3 in_color;
};

struct TriangleVert {
    TriangleVert() {}

    void append_push_constant_data(std::vector<PushConstantData>& pcs) const {}
};