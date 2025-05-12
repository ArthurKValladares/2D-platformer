#pragma once

#include <filesystem>

#include "tile_types.h"

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