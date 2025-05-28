#include "map.h"

#include <fstream>
#include <print>
#include <unordered_set>
#include <utility>

MapLayout::MapLayout(const std::filesystem::path& path) {
    std::fstream map_file;
	map_file.open(path, std::ios::in);
	if (!map_file) {
		std::print("Could not open file at: {}\n", path.string());
	}
    std::print("Loaded map at: {}\n", path.string());

    start = {-1, -1};
    end = {-1, -1};

    std::string line;

    int core_row = -1;
    int core_col = -1;

    int num_cols = -1;
    int row = 0;
    while (std::getline(map_file, line)) {
        const int num_cols_line = line.length();
        if (num_cols == -1) {
            num_cols = num_cols_line;
        }
        assert(num_cols == num_cols_line && "All lines in map must have the same number of tiles.");

        std::vector<TileType> tiles_this_line;
        tiles_this_line.reserve(num_cols);
        for (int col = 0; col < num_cols; ++col) {
            const char& c = line[col];
            const TileType ty = char_to_tile_type(c);

            if (ty == TileType::Start) {
                assert((start.row == -1 && start.col == -1) && "Maps can only have one start position.");
                start = {row, col};
            }

            if (ty == TileType::End) {
                assert((end.row == -1 && end.col == -1) && "Maps can only have one start position.");
                end = {row, col};
            }

            tiles_this_line.emplace_back(ty);
        }
        tiles.emplace_back(std::move(tiles_this_line));

        ++row;
    }

    assert((row > 0 && num_cols > 0) && "Map must have at least one tile");
    assert((start.row != -1 && start.col != -1) && "Map must have a start.");
    assert((end.row != -1 && end.col != -1) && "Map must have a end.");

    //  Reverse the map so that it matches the human-readable format, maybe not necessary later
    std::reverse(tiles.begin(), tiles.end());   
    start.row = row - 1 - start.row;
    end.row = row - 1 - end.row;
}

OptimizedMap MapLayout::optimize() const {
    OptimizedMap optimized = {};

    std::vector<MergedTile> opt_tiles;
    for (uint32_t r = 0; r < tiles.size(); ++r) {

        MergedTile curr_tile = MergedTile {
            .ty = tiles[r][0],
            .x_offset = 0,
            .y_offset = r,
            .height = 1,
            .width = 1
        };
        for (uint32_t c = 1; c < tiles[r].size(); ++c) {
            if (tiles[r][c] == curr_tile.ty) {
                ++curr_tile.width;
            } else {
                opt_tiles.push_back(curr_tile);
                curr_tile = MergedTile {
                    .ty = tiles[r][c],
                    .x_offset = c,
                    .y_offset = r,
                    .height = 1,
                    .width = 1
                };
            }
        }
        opt_tiles.push_back(curr_tile);
    }

    optimized = OptimizedMap {
        .tiles = opt_tiles,
        .start = start,
        .end = end
    };

    return optimized;
}