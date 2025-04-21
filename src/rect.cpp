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

    const float quad_vertices[] = {
        // Pos                                    // Color
        mid_x - half_size.x, mid_y - half_size.y, 0.0f, 0.0f, 1.0f, 0.0f,
        mid_x + half_size.x, mid_y + half_size.y, 0.0f, 1.0f, 0.0f, 0.0f,
        mid_x - half_size.x, mid_y + half_size.y, 0.0f, 0.0f, 0.0f, 0.0f,
        mid_x + half_size.x, mid_y - half_size.y, 0.0f, 1.0f, 1.0f, 0.0f,
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

bool Rect2D::intersects(const Rect2D& o) const {
    if (abs(pos.x - o.pos.x) > (half_size.x + o.half_size.x)) return false;
    if (abs(pos.y - o.pos.y) > (half_size.y + o.half_size.y)) return false;
    return true;
}

Rect2D Rect2D::scaled_by(float scale) const {
    return Rect2D {
        pos,
        half_size * glm::vec2(scale)
    };
}