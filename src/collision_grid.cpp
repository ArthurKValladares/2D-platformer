#include "collision_grid.h"
namespace {
    struct CellBounds {
        int32_t min_col;
        int32_t max_col;
        int32_t min_row;
        int32_t max_row;
    };

    CellBounds get_cell_bounds(Rect2D rect, float cell_size_x, float cell_size_y) {
        return CellBounds{
            .min_col = (int32_t) floor(rect.min_x() / cell_size_x),
            .max_col = (int32_t) ceil(rect.max_x() / cell_size_x),
            .min_row = (int32_t) floor(rect.min_y() / cell_size_y),
            .max_row = (int32_t) ceil(rect.max_y() / cell_size_y),
        };
    }
};

void CollisionGrid::insert_rect(Rect2D rect) {
    const CellBounds bounds = get_cell_bounds(rect, cell_size_x, cell_size_y);

    for (int32_t row = bounds.min_row; row <= bounds.max_row; ++row) {
        for (int32_t col = bounds.min_col; col <= bounds.max_col; ++col) {
            cells[Cell{row, col}].push_back(rect);
        }
    }
}

glm::vec2 CollisionGrid::get_collisions(Rect2D rect, glm::vec2 init_displacement, std::vector<CollisionGrid::CollisionData>* collisions) const {
    glm::vec2 non_colliding_disp = init_displacement;
    const CellBounds bounds = get_cell_bounds(rect, cell_size_x, cell_size_y);

    for (int32_t row = bounds.min_row; row <= bounds.max_row; ++row) {
        for (int32_t col = bounds.min_col; col <= bounds.max_col; ++col) {
            const Cell c = Cell{ row, col };
            if (cells.count(c)) {
                const std::vector<Rect2D>& cell_rects = cells.at(c);
                for (const Rect2D& cell_rect : cell_rects) {
                    Rect2D displaced_rect = rect;
                    displaced_rect.pos += non_colliding_disp;

                    if (displaced_rect.intersects(cell_rect)) {
                        const glm::vec2 intersection = displaced_rect.intersection_vector(cell_rect);
                        non_colliding_disp -= intersection;

                        if (collisions != nullptr) {
                            CollisionData data = CollisionData {
                                .rect = cell_rect,
                                .collision_size = intersection
                            };
                            collisions->push_back(data);
                        }
                    }
                }
            }
        }
    }

    return non_colliding_disp;
}