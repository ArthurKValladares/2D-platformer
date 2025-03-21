#pragma once

#include <stdint.h>

#include "renderer/resource_ids.h"

/*[[[cog
import cog
import re
from os import listdir
from os.path import isfile, join

filters = ["png"]

shader_path = "shaders"
texture_path = "assets/textures"

def get_formatted_names(dir):
    file_names = [f for f in listdir(dir) if isfile(join(dir, f))]
    file_splits = [re.split(r'[_.]+', file_name) for file_name in file_names]
    capitalized_splits = [[section.capitalize() for section in split if filters.count(section) == 0] for split in file_splits]
    return [''.join(split) for split in capitalized_splits]

shaders = get_formatted_names(shader_path)
textures = get_formatted_names(texture_path)

]]]*/
//[[[end]]]

enum class TextureSource : uint32_t {
    None = 0,
/*[[[cog
for texture in textures:
    cog.outl("%s," % texture)
]]]*/
//[[[end]]]
    Count
};
const char* texture_path(TextureSource texture);
TextureID texture_id(TextureSource texture);

enum class ShaderSource : uint32_t {
    None = 0,
/*[[[cog
for shader in shaders:
    cog.outl("%s," % shader)
]]]*/
//[[[end]]]
    Count
};
const char* shader_path(ShaderSource shader);
ShaderID shader_id(ShaderSource shader);
//
//
//