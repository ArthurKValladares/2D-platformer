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

std::vector<GridItem> CollisionGrid::get_collisions(Rect2D rect) const {
    std::vector<GridItem> collisions;

    const CellBounds bounds = get_cell_bounds(rect, cell_size_x, cell_size_y);

    for (int32_t row = bounds.min_row; row <= bounds.max_row; ++row) {
        for (int32_t col = bounds.min_col; col <= bounds.max_col; ++col) {
            const Cell c = Cell{ row, col };
            if (cells.count(c)) {
                const std::vector<GridItem>& cell_rects = cells.at(c);
                for (const GridItem& item : cell_rects) {
                    if (rect.intersects(item.rect)) {
                        collisions.push_back(item);
                    }
                }
            }
        }
    }

    return collisions;
}