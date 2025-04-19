#pragma once

#include <cassert>
#include <glm/vec3.hpp>

/*[[[cog
import cog
cases = [
    ['Path', ' ', 207, 207, 207],
    ['Wall', '#', 43, 43, 43],
    ['Start', 'S', 0, 255, 0],
    ['End', 'E', 255, 0, 0]
]
]]]*/
//[[[end]]]

enum class TileType : unsigned char {
    Invalid = 0,
    /*[[[cog
    for case in cases:
        tile_type = case[0]
        cog.outl("%s," % tile_type)
    ]]]*/
    Path,
    Wall,
    Start,
    End,
    //[[[end]]]
};

static char tile_type_to_char(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            tile_char = case[1]
            cog.outl("case TileType::%s: { return '%s'; }" % (tile_type, tile_char))
        ]]]*/
        case TileType::Path: { return ' '; }
        case TileType::Wall: { return '#'; }
        case TileType::Start: { return 'S'; }
        case TileType::End: { return 'E'; }
        //[[[end]]]
        default: {
            assert(false && "Tile type not supported");
            return '\0';
        }
    }
}

static TileType char_to_tile_type(char c) {
    switch (c) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            tile_char = case[1]
            cog.outl("case '%s': { return TileType::%s; }" % (tile_char, tile_type))
        ]]]*/
        case ' ': { return TileType::Path; }
        case '#': { return TileType::Wall; }
        case 'S': { return TileType::Start; }
        case 'E': { return TileType::End; }
        //[[[end]]]
        default: {
            assert(false &&"Char does not match a TileType");
            return TileType::Invalid;
        }
    }
}

static glm::vec3 tile_type_to_color(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            r = case[2]
            g = case[3]
            b = case[4]
            cog.outl("case TileType::%s: { return glm::vec3(%d / 255., %d / 255., %d / 255.); }" % (tile_type, r, g, b))
        ]]]*/
        case TileType::Path: { return glm::vec3(207 / 255., 207 / 255., 207 / 255.); }
        case TileType::Wall: { return glm::vec3(43 / 255., 43 / 255., 43 / 255.); }
        case TileType::Start: { return glm::vec3(0 / 255., 255 / 255., 0 / 255.); }
        case TileType::End: { return glm::vec3(255 / 255., 0 / 255., 0 / 255.); }
        //[[[end]]]
        default: {
            assert(false && "Tile type not supported");
            return glm::vec3(1.0, 0.0, 1.0);
        }
    }
}