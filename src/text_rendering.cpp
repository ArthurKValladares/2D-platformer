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

void FontFace::set_pixel_size(uint32_t width, uint32_t height) {
    FT_Error error;
    error = FT_Set_Pixel_Sizes(face, width, height);
    assert(error == 0);
}

void FontFace::setup_atlas() {
    FT_Error error;
    for (unsigned char c = 0; c < 128; c++) {
        error = FT_Load_Char(face, c, FT_LOAD_RENDER);
        assert(error == 0);

        // TODO:
    }

    error = FT_Done_Face(face);
    assert(error == 0);
}