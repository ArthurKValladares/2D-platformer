#include "rect.h"

#include "util.h"

void Rect2D::vertex_data(std::vector<QuadVertex>& vertex_buffer) const {
    const float half_size_x = (max_x - min_x) / 2.0;
    const float half_size_y = (max_y - min_y) / 2.0;

    const float mid_x = min_x + half_size_x;
    const float mid_y = min_y + half_size_y;

    const QuadVertex quad_vertices[] = {
        QuadVertex{Vec3f32{mid_x - half_size_x, mid_y - half_size_y, 0.0f}, Vec3f32{0.0f, 1.0f, 0.0f}},
        QuadVertex{Vec3f32{mid_x + half_size_x, mid_y + half_size_y, 0.0f}, Vec3f32{1.0f, 0.0f, 0.0f}},
        QuadVertex{Vec3f32{mid_x - half_size_x, mid_y + half_size_y, 0.0f}, Vec3f32{0.0f, 0.0f, 0.0f}},
        QuadVertex{Vec3f32{mid_x + half_size_x, mid_y - half_size_y, 0.0f}, Vec3f32{1.0f, 1.0f, 0.0f}},
   };

   std::move(&quad_vertices[0], &quad_vertices[ArrayCount(quad_vertices)], back_inserter(vertex_buffer));
}

void Rect2D::index_data(uint32_t vertex_offset, std::vector<uint32_t>& index_buffer) const {
    const uint32_t quad_indices[] = {
        vertex_offset, vertex_offset + 1, vertex_offset + 2,
        vertex_offset, vertex_offset + 3, vertex_offset + 1
    };
    std::move(&quad_indices[0], &quad_indices[ArrayCount(quad_indices)], back_inserter(index_buffer));
}