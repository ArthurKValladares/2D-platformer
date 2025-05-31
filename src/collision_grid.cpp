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

void CollisionGrid::insert_rect(Rect2D rect, TileType ty) {
    const CellBounds bounds = get_cell_bounds(rect, cell_size_x, cell_size_y);

    for (int32_t row = bounds.min_row; row <= bounds.max_row; ++row) {
        for (int32_t col = bounds.min_col; col <= bounds.max_col; ++col) {
            cells[Cell{row, col}].push_back(GridItem{rect, ty});
        }
    }
}

std::vector<CollisionGrid::CollisionData> CollisionGrid::get_collisions(Rect2D rect, glm::vec2 displacement) const {
    std::vector<CollisionGrid::CollisionData> collisions;

    const CellBounds bounds = get_cell_bounds(rect, cell_size_x, cell_size_y);

    for (int32_t row = bounds.min_row; row <= bounds.max_row; ++row) {
        for (int32_t col = bounds.min_col; col <= bounds.max_col; ++col) {
            const Cell c = Cell{ row, col };
            if (cells.count(c)) {
                const std::vector<GridItem>& cell_rects = cells.at(c);
                for (const GridItem& item : cell_rects) {
                    Rect2D displaced_rect = rect;
                    displaced_rect.pos += displacement;

                    if (displaced_rect.intersects(item.rect)) {

                        float x_dist;
                        if (displacement.x > 0.0) {
                            // Positive x displacement means movement to the right, so we need to test
                            // the right side of our rect vs the left side of the item we would be colliding against
                            x_dist = item.rect.min_x() - rect.max_x();
                        } else {
                            // Negative x displacement means movement to the left, so we need to test
                            // the left side of our rect vs the right side of the item we would be colliding against
                            x_dist =  rect.min_x() - item.rect.max_x();
                        }
                        const float x_relative_dist = displacement.x != 0.0
                            ? std::abs(x_dist / displacement.x)
                            : std::numeric_limits<float>::max();

                        float y_dist;
                        if (displacement.y > 0.0) {
                            // Positive y displacement means movement up, so we need to test
                            // the top side of our rect vs the bottom side of the item we would be colliding against
                            y_dist = item.rect.min_y() - rect.max_y();
                        } else {
                            // Negative y displacement means movement down, so we need to test
                            // the bottom side of our rect vs the top side of the item we would be colliding against
                            y_dist =  rect.min_y() - item.rect.max_y();
                        }
                        const float y_relative_dist = displacement.y != 0.0 
                            ? std::abs(y_dist / displacement.y)
                            : std::numeric_limits<float>::max();

                        CollisionData data = CollisionData {
                            .rect = item.rect,
                            .ty = item.ty
                        };
                        if (x_relative_dist < y_relative_dist) {
                            if (displacement.x > 0.0) {
                                data.wall = CollisionWall::Left;
                            } else {
                                data.wall = CollisionWall::Right;
                            }
                            data.relative_dist = x_relative_dist;
                        } else {
                            if (displacement.y > 0.0) {
                                data.wall = CollisionWall::Bottom;
                            } else {
                                data.wall = CollisionWall::Top;
                            }
                            data.relative_dist = y_relative_dist;
                        }

                        collisions.push_back(data);
                    }
                }
            }
        }
    }

    return collisions;
}