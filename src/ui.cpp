#include "ui.h"

UI::UI(Renderer* renderer, const Window& window)
    : global_descriptor_set(GlobalDescriptorSetData(renderer, camera))
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
}

void UI::draw(Renderer* renderer, Renderable* renderable, const char* p_text, int x_start, int y_start, glm::vec4 color, float scale) {
    font.draw(renderable, font_tex_id, p_text, x_start, y_start, color, scale);
}