#pragma once

#include <unordered_map>
#include <vector>

#include "rect.h"
#include "hash.h"

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

struct CollisionGrid {
    CollisionGrid(float cell_size_x, float cell_size_y)
        : cell_size_x(cell_size_x)
        , cell_size_y(cell_size_y)
    {}

    void insert_rect(Rect2D rect);
    std::vector<Rect2D> get_collisions(Rect2D rect) const;

    // TODO: right now just storing the collision rect is not enough,
    // I need to also know what object it corresponds to.
    // Or maybe have some sort of callback for when the collision happens
    std::unordered_map<Cell, std::vector<Rect2D>> cells;
    float cell_size_x;
    float cell_size_y;
};