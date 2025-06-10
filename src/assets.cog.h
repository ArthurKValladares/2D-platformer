#pragma once

#include <stdint.h>
#include <cassert>

#include "renderer/resource_ids.h"

/*[[[cog
import cog
import re
from os import listdir
from os.path import isfile, join

filters = ["png", "map"]

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
    //[[[end]]]
}
inline uint32_t map_count() {
    return static_cast<uint32_t>(MapSource::Count);
}
inline MapSource map_from_uint(uint32_t i) {
    return static_cast<MapSource>(i);
}