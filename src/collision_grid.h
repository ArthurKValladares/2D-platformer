#pragma once

#include <unordered_map>
#include <vector>

#include "rect.h"
#include "hash.h"
// TODO: bad
#include "map_editor/tile_types.h"

struct Cell {
    int32_t row;
    int32_t col;

    bool operator==(const Cell &o) const {
        return row == o.row && col == o.col;
    }
};

namespace std
{
    template<>
    struct hash<Cell>
    {
        size_t operator()(const Cell& c) const
        {
            size_t h= make_hash(c.row);
            hash_combine(h, make_hash(c.col));
            return h;
        }
    };
}

// TODO: This is bad, i need a better way to handle needing to know **what** I hit
struct GridItem {
    Rect2D rect;
    TileType ty;
};

struct CollisionGrid {
    CollisionGrid(float cell_size_x, float cell_size_y)
        : cell_size_x(cell_size_x)
        , cell_size_y(cell_size_y)
    {}

    void insert_rect(Rect2D rect, TileType ty);
    std::vector<GridItem> get_collisions(Rect2D rect) const;

    std::unordered_map<Cell, std::vector<GridItem>> cells;
    float cell_size_x;
    float cell_size_y;
};