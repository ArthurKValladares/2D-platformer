#include "ui.h"

UI::UI(Renderer* renderer, const Window& window)
    : global_descriptor_set(GlobalDescriptorSetData(renderer, camera))
    , draw_debug(false)
{
    const Size2Di32 window_size = window.get_size();
    camera = OrthographicCamera(
        glm::vec2(0.0),
        glm::vec2(window_size.width, window_size.height)
    );

    global_descriptor_set.shader_data.proj_matrix = camera.get_proj_matrix();
    global_descriptor_set.write_shader_data_to_buffer(renderer);

    font = text_renderer.load_font_face("C:/Windows/Fonts/arial.ttf");
    font.set_pixel_size(128);
    font_tex_id = renderer->request_texture();
    renderer->upload_texture(font_tex_id, font.setup_atlas());

    global_descriptor_set.write_shader_data_to_buffer(renderer);
    update_global_set(renderer, global_descriptor_set.buffer_id, global_descriptor_set.set_id);
}

TextSize UI::get_text_size(const char* p_text, float scale) {
    return font.get_text_size(font_tex_id, p_text, scale);
}

Renderable UI::draw(Renderer* renderer, const char* p_text, int x_start, int y_start, float scale, glm::vec4 color) {
    Renderable renderable;
    
    const TextSize text_size = font.draw(&renderable, font_tex_id, p_text, x_start, y_start, scale, color);
    if (draw_debug) {
        renderable.push_child(flat_color_quad(
            Rect2D::from_bottom_left_and_size(glm::vec2(x_start, y_start), glm::vec2(text_size.x, text_size.y)),
            glm::vec4(1.0, 0.0, 0.0, 0.25)
        ));
        renderable.push_child(flat_color_quad(
            Rect2D::from_bottom_left_and_size(glm::vec2(x_start, y_start - text_size.descender), glm::vec2(text_size.x, text_size.descender)),
            glm::vec4(0.0, 1.0, 0.0, 0.25)
        ));
    }
    return renderable;
}

void UI::draw_imgui() {
    ImGui::Checkbox("Draw Debug", &draw_debug);
}