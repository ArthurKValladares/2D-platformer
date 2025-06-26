#pragma once

#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H

struct FontFace {
    void set_pixel_size(uint32_t width, uint32_t height);

    void setup_atlas();
    FT_Face face;
};

struct TextRenderer {
    TextRenderer();

    FontFace load_font_face(const char* path);

    FT_Library library;
};