#pragma once

#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Glyph {
    Glyph() {}
    Glyph(FT_Face face, FT_UInt glyph_index);

    FT_UInt glyph_index;

    FT_Int bmp_left;
    FT_Int bmp_top;

    FT_Pos width;
    FT_Pos height;

    FT_Vector advance;
};

struct FontFace {
    void set_pixel_size(uint32_t width, uint32_t height);

    void setup_atlas();
    
    FT_Face face;
    Glyph glyphs[128];
};

struct TextRenderer {
    TextRenderer();

    FontFace load_font_face(const char* path);

    FT_Library library;
};