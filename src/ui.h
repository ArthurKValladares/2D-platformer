#pragma once

#include "window.h"
#include "renderer/renderer.h"
#include "global_descriptor_set.h"
#include "text_rendering.h"

struct UI {
    UI(Renderer* renderer, const Window& window);

    void draw(Renderer* renderer, Renderable* renderable, const char* p_text, int x_start, int y_start, glm::vec4 color, float scale);

    OrthographicCamera camera;
    GlobalDescriptorSetData global_descriptor_set;

    TextRenderer text_renderer;
    FontFace font;
    TextureID font_tex_id;
};