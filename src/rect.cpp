#include "rect.h"

#include "util.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp> 

bool Rect2D::is_zero_sized() const {
    return glm::length2(half_size) == 0.;
}

uint64_t Rect2D::vertex_data(std::vector<float>& vertex_buffer) const {
    const float mid_x = pos.x;
    const float mid_y = pos.y;

    const float uv_scale_x = half_size.x / uv_half_size.x;
    const float uv_scale_y = half_size.y / uv_half_size.y;

    const float quad_vertices[] = {
        // Pos                                          // UV
        mid_x - half_size.x, mid_y - half_size.y, 0.0f, 0.0f,       uv_scale_y, 0.0f,
        mid_x + half_size.x, mid_y + half_size.y, 0.0f, uv_scale_x, 0.0f,       0.0f, 
        mid_x - half_size.x, mid_y + half_size.y, 0.0f, 0.0f,       0.0f,       0.0f,
        mid_x + half_size.x, mid_y - half_size.y, 0.0f, uv_scale_x, uv_scale_y, 0.0f
   };

   std::move(&quad_vertices[0], &quad_vertices[ArrayCount(quad_vertices)], back_inserter(vertex_buffer));

   return ArrayCount(quad_vertices);
}

uint64_t Rect2D::index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) const {
    const uint32_t quad_indices[] = {
        vertex_offset, vertex_offset + 1, vertex_offset + 2,
        vertex_offset, vertex_offset + 3, vertex_offset + 1
    };
    std::move(&quad_indices[0], &quad_indices[ArrayCount(quad_indices)], back_inserter(index_buffer));

    return ArrayCount(quad_indices);
}

bool Rect2D::intersects_point(glm::vec2 point) const {
    return intersects(Rect2D(point, glm::vec2(0.0)));
}

bool Rect2D::intersects(const Rect2D& o) const {
    if (abs(pos.x - o.pos.x) >= (half_size.x + o.half_size.x)) return false;
    if (abs(pos.y - o.pos.y) >= (half_size.y + o.half_size.y)) return false;
    return true;
}


glm::vec2 Rect2D::intersection_vector(const Rect2D& o) const {
    const glm::vec2 center_delta = {
        o.pos.x - pos.x,
        o.pos.y - pos.y
    };

    const float overlap_x = half_size.x + o.half_size.x - std::abs(center_delta.x);
    const float overlap_y = half_size.y + o.half_size.y - std::abs(center_delta.y);

    if (overlap_x <= 0. || overlap_y <= 0.) {
        return glm::vec2(0.0f, 0.0f);
    }

    if (overlap_x < overlap_y) {
        return glm::vec2(
            (center_delta.x < 0.f) ? -overlap_x : overlap_x, 
            0.0f
        );
    } else {
        return glm::vec2(
            0.0f,
            center_delta.y < 0.f ? -overlap_y : overlap_y
        );
    }
}

Rect2D Rect2D::scaled_by(float x_scale, float y_scale) const {
    return Rect2D(pos,half_size * glm::vec2(2.0 * x_scale, 2.0 * y_scale));
}

Rect2D Rect2D::merge(Rect2D o) const {
    const float merged_min_x = std::min(min_x(), o.min_x());
    const float merged_max_x = std::max(max_x(), o.max_x());

    const float merged_min_y = std::min(min_y(), o.min_y());
    const float merged_max_y = std::max(max_y(), o.max_y());

    return Rect2D::from_min_max(merged_min_x, merged_max_x, merged_min_y, merged_max_y);
}