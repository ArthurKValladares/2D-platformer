#pragma once

#include "point.h"
#include "size.h"
#include "vec.h"

#include <vector>

struct QuadVertex {
    Vec3f32 pos;
    Vec3f32 uv;
};

struct Rect2D {
    Rect2D() {}
    Rect2D(Point2Df32 center, Size2Df32 size)
        : Rect2D(
            center.x - size.width / 2.f,
            center.x + size.width / 2.f,
            center.y - size.height / 2.f,
            center.y + size.height / 2.f
        )
    {}
    Rect2D(float min_x, float max_x, float min_y, float max_y) 
        : min_x(min_x)
        , max_x(max_x)
        , min_y(min_y)
        , max_y(max_y)
    {}

    bool is_zero_sized() const {
        return min_x == max_x || min_y == max_y;
    }

    uint64_t vertex_data(std::vector<QuadVertex>& vertex_buffer) const;
    uint64_t index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) const;

    float min_x = 0.0, max_x = 0.0;
    float min_y = 0.0, max_y = 0.0;
};