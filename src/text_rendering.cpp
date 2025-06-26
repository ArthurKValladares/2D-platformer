#include "text_rendering.h"

#include <cassert>

TextRenderer::TextRenderer() {
    FT_Error error;
    error = FT_Init_FreeType(&library);
    assert(error == 0);
}

FontFace TextRenderer::load_font_face(const char* path) {
    FT_Error error;

    FT_Face face;
    error = FT_New_Face(
        library,
        path,
        0,
        &face
    );
    assert(error == 0);

    return FontFace{
        .face = face
    };
}

Glyph::Glyph(FT_Face face, FT_UInt glyph_index) 
    : glyph_index(glyph_index)
{
    FT_GlyphSlot glyp_slot = face->glyph;

    FT_Error error;

    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
    assert(error == 0);

    error = FT_Render_Glyph(glyp_slot, FT_RENDER_MODE_NORMAL);
    assert(error == 0);

    bmp_left = glyp_slot->bitmap_left;
    bmp_top = glyp_slot->bitmap_top;

    width = glyp_slot->metrics.width;
    height = glyp_slot->metrics.height;

    advance = glyp_slot->advance;
}

void FontFace::set_pixel_size(uint32_t width, uint32_t height) {
    FT_Error error;
    error = FT_Set_Pixel_Sizes(face, width, height);
    assert(error == 0);
}

void FontFace::setup_atlas() {
    FT_Error error;
    for (unsigned char c = 0; c < 128; c++) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, c);

        glyphs[c] = Glyph(face, glyph_index);
    }

    error = FT_Done_Face(face);
    assert(error == 0);
}