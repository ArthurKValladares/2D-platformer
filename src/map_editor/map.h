#pragma once

#include <filesystem>

#include "tile_types.h"

struct TilePosition {
    int row;
    int col;
};

struct MergedTile {
    TileType ty;
    uint32_t height;
    uint32_t width;
};

struct OptimizedMap {
    std::vector<std::vector<MergedTile>> tiles;
    TilePosition start;
    TilePosition end;
};

struct MapLayout {
    MapLayout() {}
    MapLayout(const std::filesystem::path& path);

    OptimizedMap optimize() const;

    std::vector<std::vector<TileType>> tiles;
    TilePosition start;
    TilePosition end;
};