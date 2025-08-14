#pragma once

#include <filesystem>

#include "tile_types.h"

struct TilePosition {
    int row;
    int col;
};

struct MergedTile {
    TileType ty;

    uint32_t x_offset;
    uint32_t y_offset;

    uint32_t height;
    uint32_t width;
};

// TODO: 'Collidable' system needs to be better
struct TilePickup {
    TilePosition pos;
    TileType ty;
    bool is_active;
};
struct TileEnemy {
    TilePosition pos;
    TileType ty;
    bool is_active;
};
struct TileHazard {
    TilePosition pos;
    TileType ty;
    bool is_active;
    glm::vec2 dir;
};

struct OptimizedMap {
    std::vector<MergedTile> tiles;
    TilePosition start;
    TilePosition end;
    uint32_t width;
    uint32_t height;
    // TODO: unecessary copy from regular map to optimized map
    std::vector<TilePickup> pickups;
    std::vector<TileEnemy> enemies;
    std::vector<TileHazard> hazards;
};

struct MapLayout {
    MapLayout() {}
    MapLayout(const std::filesystem::path& path);

    OptimizedMap optimize() const;

    std::vector<std::vector<TileType>> tiles;
    TilePosition start;
    TilePosition end;
    // TODO: unecessary copy from regular map to optimized map
    std::vector<TilePickup> pickups;
    std::vector<TileEnemy> enemies;
    std::vector<TileHazard> hazards;
};