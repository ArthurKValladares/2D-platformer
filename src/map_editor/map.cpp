#include "map.h"

#include <fstream>
#include <print>
#include <unordered_map>
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

    assert((start.row != -1 && start.col != -1) && "Map must have a start.");
    assert((end.row != -1 && end.col != -1) && "Map must have a end.");
}