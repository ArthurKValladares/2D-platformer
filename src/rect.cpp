#include "rect.h"

std::vector<Vec3f32> Rect2D::vertex_data() const {
    const float half_size_x = (max_x - min_x) / 2.0;
    const float half_size_y = (max_y - min_y) / 2.0;

    const float mid_x = min_x + half_size_x;
    const float mid_y = min_y + half_size_y;

    return {
        Vec3f32{mid_x - half_size_x, mid_y - half_size_y, 0.0f}, Vec3f32{0.0f, 1.0f, 0.0f},
        Vec3f32{mid_x + half_size_x, mid_y + half_size_y, 0.0f}, Vec3f32{1.0f, 0.0f, 0.0f},
        Vec3f32{mid_x - half_size_x, mid_y + half_size_y, 0.0f}, Vec3f32{0.0f, 0.0f, 0.0f},
        Vec3f32{mid_x + half_size_x, mid_y - half_size_y, 0.0f}, Vec3f32{1.0f, 1.0f, 0.0f},
   };
}