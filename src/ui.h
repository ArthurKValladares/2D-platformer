#pragma once

#include "window.h"
#include "renderer/renderer.h"
#include "global_descriptor_set.h"
#include "text_rendering.h"

struct UI {
    UI(Renderer* renderer, const Window& window);

    glm::vec3 get_text_size(const char* p_text, float scale);
    Renderable draw(Renderer* renderer, const char* p_text, int x_start, int y_start, float scale, glm::vec4 color);
    void draw_imgui();

    OrthographicCamera camera;
    GlobalDescriptorSetData global_descriptor_set;

    TextRenderer text_renderer;
    FontFace font;
    TextureID font_tex_id;

    bool draw_debug;
};