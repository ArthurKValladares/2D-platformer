#pragma once

#include "button.h"
#include "view.h"
#include "ui.h"

struct PauseMenu {
    PauseMenu() {}
    PauseMenu(UI& ui);

    Renderable draw(UI& ui, Renderer* renderer);
    void update_fn(const UpdateContext& context, double total_elapsed_seconds, double frame_dt);

    Button useless_button_0;
    Button quit_button;
    Button useless_button_1;
};