#pragma once

#include <cassert>
#include <glm/vec3.hpp>

#include "../assets.h"

#define TILE_SIZE 1.0

/*[[[cog
import cog
cases = [
    ['Path',        ' ', 'Path', '',           [255, 255, 255], [255, 255, 255]],
    ['Wall',        '#', 'Wall', '',           [255, 255, 255], [255, 255, 255]],
    ['Start',       'S', 'Path', '',           [255, 255, 255], [255, 255, 255]],
    ['End',         'E', 'Path', 'End',        [255, 255, 255], [255, 255, 255]],
    ['DoubleJump',  'D', 'Path', 'Diamond',    [255, 255, 255], [255, 255, 255]],
    ['DoubleDash',  'F', 'Path', 'Pentagon',   [255, 255, 255], [255, 255, 255]],
    ['Spike',       '^', 'Path', 'Spike',      [255, 255, 255], [255, 255, 255]],
    ['MovingSpike', 'V', 'Path', 'Spike',      [255, 255, 255], [128, 128, 128]],
    ['BasicEnemy',  'B', 'Path', 'Enemy',      [255, 255, 255], [255, 255, 255]],
    ['SpikyEnemy',  'T', 'Path', 'SpikyEnemy', [255, 255, 255], [255, 255, 255]],
    ['SawShooter',  'U', 'Path', 'SawShooter', [255, 255, 255], [255, 255, 255]],
]

pickups = [
    'End',
    'DoubleJump',
    'DoubleDash'
]
enemies = [
    'BasicEnemy',
    'SpikyEnemy'
]
hazards = [
    'Spike',
    'MovingSpike',
    'SawShooter'
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
    DoubleDash,
    Spike,
    MovingSpike,
    BasicEnemy,
    SpikyEnemy,
    SawShooter,
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
        case TileType::DoubleDash: { return 'F'; }
        case TileType::Spike: { return '^'; }
        case TileType::MovingSpike: { return 'V'; }
        case TileType::BasicEnemy: { return 'B'; }
        case TileType::SpikyEnemy: { return 'T'; }
        case TileType::SawShooter: { return 'U'; }
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
        case 'F': { return TileType::DoubleDash; }
        case '^': { return TileType::Spike; }
        case 'V': { return TileType::MovingSpike; }
        case 'B': { return TileType::BasicEnemy; }
        case 'T': { return TileType::SpikyEnemy; }
        case 'U': { return TileType::SawShooter; }
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
        case TileType::DoubleDash: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::Spike: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::MovingSpike: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::BasicEnemy: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::SpikyEnemy: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::SawShooter: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        //[[[end]]]
        default: {
            assert(false && "Tile type not supported");
            return glm::vec3(1.0, 0.0, 1.0);
        }
    }
}

static glm::vec3 tile_type_to_item_color(TileType ty) {
    switch (ty) {
        /*[[[cog
        for case in cases:
            tile_type = case[0]
            rgb = case[5]
            cog.outl("case TileType::%s: { return glm::vec3(%d / 255., %d / 255., %d / 255.); }" % (tile_type, rgb[0], rgb[1], rgb[2]))
        ]]]*/
        case TileType::Path: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::Wall: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::Start: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::End: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::DoubleJump: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::DoubleDash: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::Spike: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::MovingSpike: { return glm::vec3(128 / 255., 128 / 255., 128 / 255.); }
        case TileType::BasicEnemy: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::SpikyEnemy: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
        case TileType::SawShooter: { return glm::vec3(255 / 255., 255 / 255., 255 / 255.); }
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
        case TileType::DoubleDash: { return TextureSource::Path; }
        case TileType::Spike: { return TextureSource::Path; }
        case TileType::MovingSpike: { return TextureSource::Path; }
        case TileType::BasicEnemy: { return TextureSource::Path; }
        case TileType::SpikyEnemy: { return TextureSource::Path; }
        case TileType::SawShooter: { return TextureSource::Path; }
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
        case TileType::DoubleDash: { return TextureSource::Pentagon; }
        case TileType::Spike: { return TextureSource::Spike; }
        case TileType::MovingSpike: { return TextureSource::Spike; }
        case TileType::BasicEnemy: { return TextureSource::Enemy; }
        case TileType::SpikyEnemy: { return TextureSource::SpikyEnemy; }
        case TileType::SawShooter: { return TextureSource::SawShooter; }
        //[[[end]]]
        default: {
            return TextureSource::Count;
        }
    }
}

static bool is_pickup(TileType ty) {
    switch (ty) {
        /*[[[cog
        for pickup in pickups:
            cog.outl("case TileType::%s: { return true; }" % pickup)
        ]]]*/
        case TileType::End: { return true; }
        case TileType::DoubleJump: { return true; }
        case TileType::DoubleDash: { return true; }
        //[[[end]]]
        default: {
            return false;
        }
    }
}
static bool is_enemy(TileType ty) {
    switch (ty) {
        /*[[[cog
        for enemy in enemies:
            cog.outl("case TileType::%s: { return true; }" % enemy)
        ]]]*/
        case TileType::BasicEnemy: { return true; }
        case TileType::SpikyEnemy: { return true; }
        //[[[end]]]
        default: {
            return false;
        }
    }
}
static bool is_hazard(TileType ty) {
    switch (ty) {
        /*[[[cog
        for hazard in hazards:
            cog.outl("case TileType::%s: { return true; }" % hazard)
        ]]]*/
        case TileType::Spike: { return true; }
        case TileType::MovingSpike: { return true; }
        case TileType::SawShooter: { return true; }
        //[[[end]]]
        default: {
            return false;
        }
    }
}