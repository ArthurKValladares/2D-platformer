#pragma once

#include <cassert>
#include <glm/vec3.hpp>

#include "../assets.h"

#define TILE_SIZE 1.0

// TODO: Right now `End` is both a special case and not. Just make it a pickup later
/*[[[cog
import cog
cases = [
    ['Path',       ' ', 'Path', '',        [255, 255, 255]],
    ['Wall',       '#', 'Wall', '',        [255, 255, 255]],
    ['Start',      'S', 'Path', '',        [255, 255, 255]],
    ['End',        'E', 'Path', 'End',     [255, 255, 255]],
    ['DoubleJump', 'D', 'Path', 'Diamond', [255, 255, 255]]
]
]]]*/
//[[[end]]]

enum class TileType : unsigned char {
    /*[[[cog
    for case in cases:
        tile_type = case[0]
        cog.outl("%s," % tile_type)
    ]]]*/
    Path,
    Wall,
    Start,
    End,
    DoubleJump,
    //[[[end]]]
    Count
};

inline uint32_t tile_type_count() {
    return static_cast<uint32_t>(TileType::Count);
}

inline TileType tile_type_from_uint(uint32_t i) {
    return static_cast<TileType>(i);
}

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
        case TileType::DoubleJump: { return 'D'; }
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
        case 'D': { return TileType::DoubleJump; }
        //[[[end]]]
        default: {
            assert(false &&"Char does not match a TileType");
            return TileType::Count;
        }
    }
}

static glm::vec3 tile_type_to_color(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            rgb = case[4]
            cog.outl("case TileType::%s: { return glm::vec3(%d / 255., %d / 255., %d / 255.); }" % (tile_type, rgb[0], rgb[1], rgb[2]))
        ]]]*/
        case TileType::Path: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::Wall: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::Start: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::End: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::DoubleJump: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
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
        case TileType::Path: { return TextureSource::Path; }
        case TileType::Wall: { return TextureSource::Wall; }
        case TileType::Start: { return TextureSource::Path; }
        case TileType::End: { return TextureSource::Path; }
        case TileType::DoubleJump: { return TextureSource::Path; }
        //[[[end]]]
        default: {
            assert(false && "Tile type not supported");
            return TextureSource::Count;
        }
    }
}

static TextureSource tile_type_to_item_texture(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            texture = case[3]
            if texture != '':
                cog.outl("case TileType::%s: { return TextureSource::%s; }" % (tile_type, texture))
        ]]]*/
        case TileType::End: { return TextureSource::End; }
        case TileType::DoubleJump: { return TextureSource::Diamond; }
        //[[[end]]]
        default: {
            return TextureSource::Count;
        }
    }
}