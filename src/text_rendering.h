#pragma once

#include <unordered_map>

#include <glm/vec2.hpp>

#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Glyph {
    Glyph() {}
    Glyph(FT_Face face, FT_UInt glyph_index, uint32_t offset);

    FT_UInt glyph_index;

    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t offset;
    uint32_t advance;
};

struct FontFace {
    void set_pixel_size(uint32_t width, uint32_t height);

    void setup_atlas();
    
    FT_Face face;
    Glyph glyphs[128];
    std::unordered_map<char, std::vector<uint8_t>> glyph_data;
    
    uint32_t bmp_width;
    uint32_t bmp_height;
};

struct TextRenderer {
    TextRenderer();

    FontFace load_font_face(const char* path);

    FT_Library library;
};