#pragma once

#include <cassert>
#include <glm/vec3.hpp>

#include "../assets.h"

#define TILE_SIZE 1.0

/*[[[cog
import cog
cases = [
    ['Path', ' ', 'Test1', [207, 207, 207]],
    ['Wall', '#', 'Test2', [43, 43, 43]],
    ['Start', 'S', 'Akv', [0, 255, 0]],
    ['End', 'E', 'Test3', [255, 0, 0]]
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
    //[[[end]]]
    Count
};

static char tile_type_to_char(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            tile_char = case[1]
            cog.outl("case TileType::%s: { return '%s'; }" % (tile_type, tile_char))
        ]]]*/
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
            rgb = case[3]
            cog.outl("case TileType::%s: { return glm::vec3(%d / 255., %d / 255., %d / 255.); }" % (tile_type, rgb[0], rgb[1], rgb[2]))
        ]]]*/
        //[[[end]]]
        default: {
            assert(false && "Tile type not supported");
            return glm::vec3(1.0, 0.0, 1.0);
        }
    }
}

static TextureSource tile_type_to_texture(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            texture = case[2]
            cog.outl("case TileType::%s: { return TextureSource::%s; }" % (tile_type, texture))
        ]]]*/
        //[[[end]]]
        default: {
            assert(false && "Tile type not supported");
            return TextureSource::None;
        }
    }
}