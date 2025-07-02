#pragma once

#include <functional>

#include "rect.h"
#include "renderables/includes.h"
#include "ui.h"
#include "keyboard_state.h"

struct Button  {
    Button() {}
    Button(Rect2D rect, glm::vec4 button_color, glm::vec4 button_color_selected, const char* p_text, float text_scale, glm::vec4 text_color, std::function<void()> on_pressed)
        : rect(rect)
        , button_color(button_color)
        , button_color_selected(button_color_selected)
        , p_text(p_text)
        , text_scale(text_scale)
        , text_color(text_color)
        , selected(false)
        , on_pressed(on_pressed)
    {}

    Renderable draw(Renderer* renderer, UI& ui) {
        Renderable renderable;

        const glm::vec4 color = selected ? button_color_selected : button_color;
        renderable.push_child(flat_color_quad(
            rect,
            color
        ));

        TextSize text_size = ui.get_text_size(p_text, 1.0);
        float x_start = rect.center().x - text_size.x / 2.0;
        float y_start = rect.center().y - text_size.y / 2.0;
        renderable.push_child(ui.draw(renderer, p_text, x_start, y_start, text_scale, text_color));

        return renderable;
    } 

    void update(const MouseState& mouse_state, glm::vec2 mouse_pos) {
        selected = rect.intersects_point(mouse_pos);
        if (selected && mouse_state.is_down(SDL_BUTTON_LEFT)) {
            on_pressed();
        }
    }

    Rect2D rect;
    glm::vec4 button_color;
    glm::vec4 button_color_selected;
    const char* p_text;
    float text_scale;
    glm::vec4 text_color;

    bool selected;
    std::function<void()> on_pressed;
};