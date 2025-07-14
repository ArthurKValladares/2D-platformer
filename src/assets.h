#pragma once

#include <stdint.h>
#include <cassert>

#include "renderer/resource_ids.h"

// Credit for skeleton asset: https://opengameart.org/content/skeleton-animations

/*[[[cog
import cog
import re
from os import listdir
from os.path import isfile, join

filters = ["png", "jpg", "map"]

shader_path = "shaders"
texture_path = "assets/textures"
maps_path = "assets/maps"

def get_file_names(dir):
    return [f for f in listdir(dir) if isfile(join(dir, f))]

def get_formatted_names(file_names):
    file_splits = [re.split(r'[_.]+', file_name) for file_name in file_names]
    capitalized_splits = [[section.capitalize() for section in split if filters.count(section) == 0] for split in file_splits]
    return [''.join(split) for split in capitalized_splits]

shader_files = get_file_names(shader_path)
formatted_shaders = get_formatted_names(shader_files)

texture_files = get_file_names(texture_path)
formatted_textures = get_formatted_names(texture_files)

map_files = get_file_names(maps_path)
formatted_maps = get_formatted_names(map_files)

]]]*/
//[[[end]]]

//
// TEXTURES
//

enum class TextureSource : uint32_t {
    /*[[[cog
    for texture in formatted_textures:
        cog.outl("%s," % texture)
    ]]]*/
    Diamond,
    End,
    Go1,
    Go2,
    Go3,
    Go4,
    Go5,
    Go6,
    Go7,
    Go8,
    Particle,
    Path,
    Spike,
    Wall,
    //[[[end]]]
    Count
};
inline const char* texture_path(TextureSource texture) {
    switch (texture)
    {
        /*[[[cog
        for (texture, texture_file) in zip(formatted_textures, texture_files):
            cog.out("""
            case TextureSource::%s: {
                return \"%s/%s\";
            }
            """
            % (texture, texture_path, texture_file),
            dedent=True, trimblanklines=True)
        ]]]*/
        case TextureSource::Diamond: {
            return "assets/textures/diamond.png";
        }
        case TextureSource::End: {
            return "assets/textures/end.png";
        }
        case TextureSource::Go1: {
            return "assets/textures/go_1.png";
        }
        case TextureSource::Go2: {
            return "assets/textures/go_2.png";
        }
        case TextureSource::Go3: {
            return "assets/textures/go_3.png";
        }
        case TextureSource::Go4: {
            return "assets/textures/go_4.png";
        }
        case TextureSource::Go5: {
            return "assets/textures/go_5.png";
        }
        case TextureSource::Go6: {
            return "assets/textures/go_6.png";
        }
        case TextureSource::Go7: {
            return "assets/textures/go_7.png";
        }
        case TextureSource::Go8: {
            return "assets/textures/go_8.png";
        }
        case TextureSource::Particle: {
            return "assets/textures/particle.png";
        }
        case TextureSource::Path: {
            return "assets/textures/path.jpg";
        }
        case TextureSource::Spike: {
            return "assets/textures/spike.png";
        }
        case TextureSource::Wall: {
            return "assets/textures/wall.jpg";
        }
        //[[[end]]]
        case TextureSource::Count: {
            assert(false);
            return "";
        }
        default:{
            assert(false);
            return "";
        }
    }    
}
inline const char* texture_name(TextureSource texture) {
    switch (texture)
    {
        /*[[[cog
        for texture in formatted_textures:
            cog.out("""
            case TextureSource::%s: {
                return \"%s\";
            }
            """
            % (texture, texture),
            dedent=True, trimblanklines=True)
        ]]]*/
        case TextureSource::Diamond: {
            return "Diamond";
        }
        case TextureSource::End: {
            return "End";
        }
        case TextureSource::Go1: {
            return "Go1";
        }
        case TextureSource::Go2: {
            return "Go2";
        }
        case TextureSource::Go3: {
            return "Go3";
        }
        case TextureSource::Go4: {
            return "Go4";
        }
        case TextureSource::Go5: {
            return "Go5";
        }
        case TextureSource::Go6: {
            return "Go6";
        }
        case TextureSource::Go7: {
            return "Go7";
        }
        case TextureSource::Go8: {
            return "Go8";
        }
        case TextureSource::Particle: {
            return "Particle";
        }
        case TextureSource::Path: {
            return "Path";
        }
        case TextureSource::Spike: {
            return "Spike";
        }
        case TextureSource::Wall: {
            return "Wall";
        }
        //[[[end]]]
        case TextureSource::Count: {
            assert(false);
            return "";
        }
        default:{
            assert(false);
            return "";
        }
    }   
}
inline const char** texture_names() {
    /*[[[cog
    cog.outl("static const char* names[] = {");
    for texture in formatted_textures:
        cog.out("""
        \"%s\",
        """
        % texture,
        trimblanklines=True)
    cog.outl("};")
    cog.outl("return names;")
    ]]]*/
    static const char* names[] = {
        "Diamond",
        "End",
        "Go1",
        "Go2",
        "Go3",
        "Go4",
        "Go5",
        "Go6",
        "Go7",
        "Go8",
        "Particle",
        "Path",
        "Spike",
        "Wall",
    };
    return names;
    //[[[end]]]
}
inline uint32_t texture_count() {
    return static_cast<uint32_t>(TextureSource::Count);
}
inline TextureSource texture_from_uint(uint32_t i) {
    return static_cast<TextureSource>(i);
}
inline TextureID texture_id(TextureSource texture) {
    return TextureID(static_cast<uint32_t>(texture));
}

//
// SHADERS
//

enum class ShaderSource : uint32_t {
    /*[[[cog
    for shader in formatted_shaders:
        cog.outl("%s," % shader)
    ]]]*/
    FlatColorFrag,
    FlatColorVert,
    FontFrag,
    OutlineFrag,
    TriangleFrag,
    TriangleVert,
    TriangleColorFrag,
    TriangleDataFrag,
    TriangleDataVert,
    TriangleTransformVert,
    //[[[end]]]
    Count
};
inline const char* shader_path(ShaderSource shader) {
    switch (shader)
    {
        /*[[[cog
        for (shader, shader_file) in zip(formatted_shaders, shader_files):
            cog.out("""
            case ShaderSource::%s: {
                return \"%s/%s.spv\";
            }"""
            % (shader, shader_path, shader_file),
            dedent=True, trimblanklines=True)
        ]]]*/
        case ShaderSource::FlatColorFrag: {
            return "shaders/flat_color.frag.spv";
        }
        case ShaderSource::FlatColorVert: {
            return "shaders/flat_color.vert.spv";
        }
        case ShaderSource::FontFrag: {
            return "shaders/font.frag.spv";
        }
        case ShaderSource::OutlineFrag: {
            return "shaders/outline.frag.spv";
        }
        case ShaderSource::TriangleFrag: {
            return "shaders/triangle.frag.spv";
        }
        case ShaderSource::TriangleVert: {
            return "shaders/triangle.vert.spv";
        }
        case ShaderSource::TriangleColorFrag: {
            return "shaders/triangle_color.frag.spv";
        }
        case ShaderSource::TriangleDataFrag: {
            return "shaders/triangle_data.frag.spv";
        }
        case ShaderSource::TriangleDataVert: {
            return "shaders/triangle_data.vert.spv";
        }
        case ShaderSource::TriangleTransformVert: {
            return "shaders/triangle_transform.vert.spv";
        }
        //[[[end]]]
        case ShaderSource::Count: {
            assert(false);
            return "";
        }
        default:{
            assert(false);
            return "";
        }
    }
}
inline const char* shader_name(ShaderSource shader) {
    switch (shader)
    {
        /*[[[cog
        for shader in formatted_shaders:
            cog.out("""
            case ShaderSource::%s: {
                return \"%s\";
            }"""
            % (shader, shader),
            dedent=True, trimblanklines=True)
        ]]]*/
        case ShaderSource::FlatColorFrag: {
            return "FlatColorFrag";
        }
        case ShaderSource::FlatColorVert: {
            return "FlatColorVert";
        }
        case ShaderSource::FontFrag: {
            return "FontFrag";
        }
        case ShaderSource::OutlineFrag: {
            return "OutlineFrag";
        }
        case ShaderSource::TriangleFrag: {
            return "TriangleFrag";
        }
        case ShaderSource::TriangleVert: {
            return "TriangleVert";
        }
        case ShaderSource::TriangleColorFrag: {
            return "TriangleColorFrag";
        }
        case ShaderSource::TriangleDataFrag: {
            return "TriangleDataFrag";
        }
        case ShaderSource::TriangleDataVert: {
            return "TriangleDataVert";
        }
        case ShaderSource::TriangleTransformVert: {
            return "TriangleTransformVert";
        }
        //[[[end]]]
        case ShaderSource::Count: {
            assert(false);
            return "";
        }
        default:{
            assert(false);
            return "";
        }
    }
}
inline const char** shader_names() {
    /*[[[cog
    cog.outl("static const char* names[] = {");
    cog.outl("\t\"None\",")
    for shader in formatted_shaders:
        cog.out("""
        \"%s\",
        """
        % shader,
        trimblanklines=True)
    cog.outl("};")
    cog.outl("return names;")
    ]]]*/
    static const char* names[] = {
    	"None",
        "FlatColorFrag",
        "FlatColorVert",
        "FontFrag",
        "OutlineFrag",
        "TriangleFrag",
        "TriangleVert",
        "TriangleColorFrag",
        "TriangleDataFrag",
        "TriangleDataVert",
        "TriangleTransformVert",
    };
    return names;
    //[[[end]]]
}
inline uint32_t shader_count() {
    return static_cast<uint32_t>(ShaderSource::Count);
}
inline ShaderSource shader_from_uint(uint32_t i) {
    return static_cast<ShaderSource>(i);
}
inline ShaderID shader_id(ShaderSource shader) {
    return ShaderID(static_cast<uint32_t>(shader));
}

//
// MAPS
//

enum class MapSource : uint32_t {
    /*[[[cog
    for map in formatted_maps:
        cog.outl("%s," % map)
    ]]]*/
    Test,
    Test2,
    //[[[end]]]
    Count
};
inline const char* map_path(MapSource map) {
    switch (map)
    {
        /*[[[cog
        for (map, map_file) in zip(formatted_maps, map_files):
            cog.out("""
            case MapSource::%s: {
                return \"%s/%s\";
            }
            """
            % (map, maps_path, map_file),
            dedent=True, trimblanklines=True)
        ]]]*/
        case MapSource::Test: {
            return "assets/maps/test_map.map";
        }
        case MapSource::Test2: {
            return "assets/maps/test_map_2.map";
        }
        //[[[end]]]
        case MapSource::Count: {
            assert(false);
            return "";
        }
        default: {
            assert(false);
            return "";
        }
    }    
}
inline const char* map_name(MapSource map) {
    switch (map)
    {
        /*[[[cog
        for map in formatted_maps:
            cog.out("""
            case MapSource::%s: {
                return \"%s\";
            }
            """
            % (map, map),
            dedent=True, trimblanklines=True)
        ]]]*/
        case MapSource::Test: {
            return "Test";
        }
        case MapSource::Test2: {
            return "Test2";
        }
        //[[[end]]]
        case MapSource::Count: {
            assert(false);
            return "";
        }
        default:{
            assert(false);
            return "";
        }
    }   
}
inline const char** map_names() {
    /*[[[cog
    cog.outl("static const char* names[] = {");
    for map in formatted_maps:
        cog.out("""
        \"%s\",
        """
        % map,
        trimblanklines=True)
    cog.outl("};")
    cog.outl("return names;")
    ]]]*/
    static const char* names[] = {
        "Test",
        "Test2",
    };
    return names;
    //[[[end]]]
}
inline uint32_t map_count() {
    return static_cast<uint32_t>(MapSource::Count);
}
inline MapSource map_from_uint(uint32_t i) {
    return static_cast<MapSource>(i);
}