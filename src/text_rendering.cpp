#include "text_rendering.h"

#include <cassert>
#include <algorithm>
#include <vector>

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
        }

        bmp_width += face->glyph->bitmap.width;
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
            x_pos += width;
        }
    }

    return TextureCreateInfo{
        .buffer = buffer,
        .buffer_size = buffer_size,
        .width = bmp_width,
        .height = bmp_height,
        .format = VK_FORMAT_R8_UNORM,
    };
}

void FontFace::get_text_size(const char* p_text, float& width, float& height) {
    size_t len = strlen(p_text);
    float x = 0.f;
    float y = 0.f;
    for (size_t i = 0; i < len; i++) {
        char c = p_text[i];
        Glyph& glyph = glyphs[c];
        float w = (float)glyph.size.x;
        float h = (float)glyph.size.y;


        x += (glyph.advance >> 6);
        y = std::max(y, h);
    }
    width = x;
    height = y;
}

void FontFace::draw(Renderer* renderer, Renderable* renderable, BufferID global_data_buffer, TextureID font_id, const char* p_text, int x_start, int y_start, glm::vec4 color) {
    const uint64_t len = ArrayCount(p_text);
    float x = (float) x_start;
    const float y = (float) x_start;
    const float f_bmp_height = (float) bmp_height;
 
    float text_width, text_height;
    get_text_size(p_text, text_width, text_height);
    const float scale = 1.0;

    for (uint32_t i = 0; i < len; ++i) {
        const char c = p_text[i];
        const Glyph& glyph = glyphs[c];

        const float x_pos = x + glyph.bearing.x * scale;
        const float y_pos = y + (text_height - glyph.bearing.y) * scale;

        const float glyph_width = (float) glyph.size.x * scale;
        const float glyph_height = (float) glyph.size.y * scale;

        const float inv_bmp_width = 1.0 / (float) bmp_width;

        const float u0 = (float) glyph.offset * inv_bmp_width;
        const float u1 = (float) (glyph.offset + glyph.size.x) * inv_bmp_width;

        const float v0 = 0.0;
        const float v1 = glyph.size.x / f_bmp_height;

        // TODO: find a way to need to pass the renderer and global buffer around all over the place
        renderable->push_child(colored_quad(
            renderer,
            Rect2D::from_top_left_and_size(glm::vec2(x_pos, y_pos), glm::vec2(glyph_width, glyph_height), glm::vec2(u0, v0), glm::vec2(u1, v1)),
            font_id,
            color,
            global_data_buffer
        ));

        x += (glyph.advance >> 6) * scale;
    }
}