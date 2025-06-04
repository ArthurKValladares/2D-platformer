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

struct OptimizedMap {
    std::vector<MergedTile> tiles;
    TilePosition start;
    TilePosition end;
    uint32_t width;
    uint32_t height;
};

struct MapLayout {
    MapLayout() {}
    MapLayout(const std::filesystem::path& path);

    OptimizedMap optimize() const;

    std::vector<std::vector<TileType>> tiles;
    TilePosition start;
    TilePosition end;
};