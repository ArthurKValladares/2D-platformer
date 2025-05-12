#pragma once

#include <filesystem>

#include "tile_types.h"

#define TILE_SIZE 1.0

struct TilePosition {
    int row;
    int col;
};

struct MapLayout {
    MapLayout(const std::filesystem::path& path);

    void print() const;

    std::vector<std::vector<TileType>> tiles;
    TilePosition start;
    TilePosition end;
};