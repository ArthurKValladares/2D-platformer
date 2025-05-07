#pragma once

#include <stdint.h>
#include <cassert>

#include "renderer/resource_ids.h"

/*[[[cog
import cog
import re
from os import listdir
from os.path import isfile, join

filters = ["png"]

shader_path = "shaders"
texture_path = "assets/textures"

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

]]]*/
//[[[end]]]

enum class TextureSource : uint32_t {
    None = 0,
    /*[[[cog
    for texture in formatted_textures:
        cog.outl("%s," % texture)
    ]]]*/
    Akv,
    Particle,
    Test1,
    Test2,
    Test3,
    Test4,
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
        case TextureSource::Akv: {
            return "assets/textures/akv.png";
        }
        case TextureSource::Particle: {
            return "assets/textures/particle.png";
        }
        case TextureSource::Test1: {
            return "assets/textures/test_1.png";
        }
        case TextureSource::Test2: {
            return "assets/textures/test_2.png";
        }
        case TextureSource::Test3: {
            return "assets/textures/test_3.png";
        }
        case TextureSource::Test4: {
            return "assets/textures/test_4.png";
        }
        //[[[end]]]
        case TextureSource::None: {
            assert(false);
            return "";
        }
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
        case TextureSource::Akv: {
            return "Akv";
        }
        case TextureSource::Particle: {
            return "Particle";
        }
        case TextureSource::Test1: {
            return "Test1";
        }
        case TextureSource::Test2: {
            return "Test2";
        }
        case TextureSource::Test3: {
            return "Test3";
        }
        case TextureSource::Test4: {
            return "Test4";
        }
        //[[[end]]]
        case TextureSource::None: {
            assert(false);
            return "";
        }
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
    cog.outl("\t\"None\",")
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
    	"None",
        "Akv",
        "Particle",
        "Test1",
        "Test2",
        "Test3",
        "Test4",
    };
    return names;
    //[[[end]]]
}
inline uint32_t texture_count() {
    return static_cast<uint32_t>(TextureSource::Count) - 1;
}
inline TextureSource texture_from_uint(uint32_t i) {
    return static_cast<TextureSource>(i);
}
inline TextureID texture_id(TextureSource texture) {
    return TextureID(static_cast<uint32_t>(texture));
}

enum class ShaderSource : uint32_t {
    None = 0,
    /*[[[cog
    for shader in formatted_shaders:
        cog.outl("%s," % shader)
    ]]]*/
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
        case ShaderSource::None: {
            assert(false);
            return "";
        }
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
        case ShaderSource::None: {
            assert(false);
            return "";
        }
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
    return static_cast<uint32_t>(ShaderSource::Count) - 1;
}
inline ShaderSource shader_from_uint(uint32_t i) {
    return static_cast<ShaderSource>(i);
}
inline ShaderID shader_id(ShaderSource shader) {
    return ShaderID(static_cast<uint32_t>(shader));
}