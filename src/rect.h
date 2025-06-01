#pragma once

#include <vector>

#include <glm/vec2.hpp>


#include "point.h"
#include "size.h"

struct Rect2D {
    Rect2D() {}
    Rect2D(glm::vec2 center, glm::vec2 size)
        : pos(center)
        , half_size(size / glm::vec2(2.0))
    {}
    static Rect2D from_top_left_and_size(glm::vec2 top_left, glm::vec2 size) {
        const glm::vec2 center = glm::vec2(top_left.x + size.x / 2.0, top_left.y + size.y / 2.0);
        return Rect2D(center, size);
    }   

    static Rect2D from_min_max(float min_x, float max_x, float min_y, float max_y) {
        const float size_x = max_x - min_x;
        const float size_y = max_y - min_y;

        glm::vec2 center = glm::vec2(min_x + size_x / 2.0, min_y + size_y / 2.0);

        return Rect2D(center, glm::vec2(size_x, size_y));
    }

    glm::vec2 center() const {
        return pos;
    }
    glm::vec2 size() const {
        return half_size * glm::vec2(2.0);
    }

    float min_x() const {
        return pos.x - half_size.x;
    }
    float max_x() const {
        return pos.x + half_size.x;
    }

    float min_y() const {
        return pos.y - half_size.y;
    }
    float max_y() const {
        return pos.y + half_size.y;
    }

    bool is_zero_sized() const;

    bool intersects_point(glm::vec2 point) const;
    bool intersects(const Rect2D& other) const;
    glm::vec2 intersection_vector(const Rect2D& other) const;

    Rect2D scaled_by(float scale) const {
        return scaled_by(scale, scale);
    }
    Rect2D scaled_by(float x_scale, float y_scale) const;

    Rect2D merge(Rect2D other) const;

    uint64_t vertex_data(std::vector<float>& vertex_buffer) const;
    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) const;

    glm::vec2 pos;
    glm::vec2 half_size;
};