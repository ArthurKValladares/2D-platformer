#include "text_rendering.h"

#include <cassert>
#include <algorithm>
#include <vector>

namespace {
    constexpr uint32_t bmp_padding = 5;
};

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
        .face = face,
        .bmp_width = 0,
        .bmp_height = 0
    };
}

Glyph::Glyph(FT_Face face, FT_UInt glyph_index, uint32_t offset) 
    : glyph_index(glyph_index)
{
    FT_GlyphSlot glyp_slot = face->glyph;

    FT_Error error;

    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
    assert(error == 0);

    error = FT_Render_Glyph(glyp_slot, FT_RENDER_MODE_NORMAL);
    assert(error == 0);

    size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
    bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
    this->offset = offset;
    advance = static_cast<unsigned int>(face->glyph->advance.x);
}

void FontFace::set_pixel_size(uint32_t size) {
    FT_Error error;
    error = FT_Set_Pixel_Sizes(face, 0, size);
    assert(error == 0);
}

TextureCreateInfo FontFace::setup_atlas() {
    FT_Error error;

    for (unsigned char c = 0; c < 128; c++) {
        FT_UInt glyph_index = FT_Get_Char_Index(face, c);
        
        bmp_height = std::max(bmp_height, face->glyph->bitmap.rows);

        glyphs[c] = Glyph(face, glyph_index, bmp_width);

        if (face->glyph->bitmap.width > 0) {
            const uint32_t pitch = face->glyph->bitmap.pitch;
            const uint32_t rows = face->glyph->bitmap.rows;
            const uint32_t width = face->glyph->bitmap.width;

            std::vector<uint8_t> char_data(rows * width);
            
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < width; j++) {
                    uint8_t byte = face->glyph->bitmap.buffer[i * pitch + j];
                    char_data[i * pitch + j] = byte;
                }
            }

            glyph_data.insert(std::pair<char, std::vector<uint8_t>>(c, char_data));

            bmp_width += face->glyph->bitmap.width + bmp_padding;
        }
    }

    error = FT_Done_Face(face);
    assert(error == 0);

    // Create Texture
    const uint64_t buffer_size = bmp_height * bmp_width;
    uint8_t* buffer = new uint8_t[buffer_size];
    memset(buffer, 0, buffer_size);

    uint32_t x_pos = 0;
    for (unsigned char c = 0; c < 128; c++)
    {
        if (glyph_data.contains(c)) {
            const Glyph& glyph = glyphs[c];
            const std::vector<uint8_t>& data = glyph_data[c];

            uint32_t width = glyph.size.x;
            uint32_t height = glyph.size.y;
            for (uint32_t i = 0; i < height; i++) {
                for (uint32_t j = 0; j < width; j++) {
                    uint8_t byte = data[i * width + j];
                    buffer[i * bmp_width + x_pos + j] = byte;
                }
            }
            x_pos += width + bmp_padding;
        }
    }

    return TextureCreateInfo{
        .buffer = buffer,
        .buffer_size = buffer_size,
        .width = bmp_width,
        .height = bmp_height,
        .format = VK_FORMAT_R8_UNORM,
        .address_mode_u = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .address_mode_v = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
    };
}

glm::vec3 FontFace::get_text_size(TextureID font_id, const char* p_text, float scale) {
    return draw(nullptr, font_id, p_text, 0, 0, 1.0, glm::vec4(1.0));
}

glm::vec3 FontFace::draw(Renderable* renderable, TextureID font_id, const char* p_text, int x_start, int y_start, float scale, glm::vec4 color) {
    const uint64_t len = strlen(p_text);
 
    float max_y = 0.0;
    float max_descender = 0.0;

    float curr_x = (float) x_start;
    for (uint32_t i = 0; i < len; ++i) {
        const char c = p_text[i];
        const Glyph& glyph = glyphs[c];

        const float glyph_width = (float) glyph.size.x * scale;
        const float glyph_height = (float) glyph.size.y * scale;
        const float glyph_y_bearing = (float) glyph.bearing.y * scale;

        max_y = std::max(max_y, glyph_height);
        max_descender = std::max(max_descender, glyph_height - glyph_y_bearing);

        if (renderable != nullptr) {
            const float x_pos = curr_x + glyph.bearing.x * scale;
            const float y_pos = y_start - (glyph.size.y - glyph.bearing.y) * scale;

            const float u0 =  glyph.offset / (float) bmp_width;
            const float u1 = (glyph.offset + glyph.size.x) / (float) bmp_width;

            const float v0 = 0.0;
            const float v1 = glyph.size.y / (float) bmp_height;

            renderable->push_child(font(
                Rect2D::from_bottom_left_and_size(glm::vec2(x_pos, y_pos), glm::vec2(glyph_width, glyph_height), glm::vec2(u0, v0), glm::vec2(u1, v1)),
                font_id,
                color
            ));
        }

        curr_x += (glyph.advance >> 6) * scale;
    }

    return glm::vec3(curr_x - x_start, max_y, max_descender);
}