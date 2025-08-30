#include "pause_menu.h"
#include "helpers.h"

PauseMenu::PauseMenu(UI& ui) {
    // button
    const char* p_text = "Button";
    const float text_scale = 1.0;
    const TextSize first_line_size = ui.get_text_size(p_text, text_scale);
    const float padding = first_line_size.y * 0.5;
    const float size_x = first_line_size.x + padding;
    const float size_y = first_line_size.y + padding;
    const glm::vec2 button_size = glm::vec2(size_x, size_y);
    const float button_distance = size_y + padding * 2;

    useless_button_0 = Button(
        Rect2D(glm::vec2(0.0) - glm::vec2(0.0, button_distance), button_size),
        glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
        glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
        p_text,
        text_scale,
        glm::vec4(1.0),
        []() {}
    );
    quit_button = Button(
        Rect2D(glm::vec2(0.0), button_size),
        glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
        glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
        "Quit",
        text_scale,
        glm::vec4(1.0),
        []() { send_quit_event(); }
    );
    useless_button_1 = Button(
        Rect2D(glm::vec2(0.0) + glm::vec2(0.0, button_distance), button_size),
        glm::vec4(128.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0, 1.0),
        glm::vec4(199.0 / 255.0, 199.0 / 255.0, 199.0 / 255.0, 1.0),
        p_text,
        text_scale,
        glm::vec4(1.0),
        []() {}
    );
}

void PauseMenu::update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt) {
    useless_button_0.update(context.mouse_state, get_screen_pos(context.window, context.mouse_state, context.ui.camera));
    quit_button.update(context.mouse_state, get_screen_pos(context.window, context.mouse_state, context.ui.camera));
    useless_button_1.update(context.mouse_state, get_screen_pos(context.window, context.mouse_state, context.ui.camera));
}

Renderable PauseMenu::draw(UI& ui, Renderer* renderer) {
    Renderable renderable;

    renderable.push_child(useless_button_0.draw(renderer, ui));
    renderable.push_child(quit_button.draw(renderer, ui));
    renderable.push_child(useless_button_1.draw(renderer, ui));

    return renderable;        
}