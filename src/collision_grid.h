#pragma once

#include <unordered_map>
#include <vector>

#include "rect.h"
#include "hash.h"
// TODO: depending on tile types is bad
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

    enum class CollisionWall {
        Left,
        Right,
        Top,
        Bottom
    };
    static const char* wall_to_string(CollisionWall wall) {
        switch (wall)
        {
            case CollisionWall::Left: {
                return "left";
            }
            case CollisionWall::Right: {
                return "right";
            }
            case CollisionWall::Top: {
                return "top";
            }
            case CollisionWall::Bottom: {
                return "bottom";
            }
        }
    }
    struct CollisionData {
        Rect2D rect;
        TileType ty;
        CollisionWall wall;
        float relative_dist;
    };
    std::vector<CollisionData> get_collisions(Rect2D rect, glm::vec2 displacement) const;

    std::unordered_map<Cell, std::vector<GridItem>> cells;
    float cell_size_x;
    float cell_size_y;
};