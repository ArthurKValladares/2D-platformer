#pragma once

#include <vector>

#include <glm/vec2.hpp>


#include "point.h"
#include "size.h"

struct Rect2D {
    Rect2D() {}
    Rect2D(glm::vec2 center, glm::vec2 size)
        : pos(center)
        , size(size)
    {}

    glm::vec2 center() const {
        return pos;
    }

    bool is_zero_sized() const;

    uint64_t vertex_data(std::vector<float>& vertex_buffer) const;
    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) const;

    glm::vec2 pos;
    glm::vec2 size;
};